package gost3410

import (
	"errors"
	"math/big"
)

type PublicKey struct {
	c    *Curve
	mode Mode
	x    *big.Int
	y    *big.Int
}

func NewPublicKey(curve *Curve, mode Mode, raw []byte) (*PublicKey, error) {
	if len(raw) != 2*int(mode) {
		return nil, errors.New("Invalid public key length")
	}
	key := make([]byte, 2*int(mode))
	copy(key, raw)
	reverse(key)
	return &PublicKey{
		curve,
		mode,
		bytes2big(key[int(mode) : 2*int(mode)]),
		bytes2big(key[:int(mode)]),
	}, nil
}

func (pub *PublicKey) Raw() []byte {
	raw := append(
		pad(pub.y.Bytes(), int(pub.mode)),
		pad(pub.x.Bytes(), int(pub.mode))...,
	)
	reverse(raw)
	return raw
}

func (pub *PublicKey) VerifyDigest(digest, signature []byte) (bool, error) {
	if len(signature) != 2*int(pub.mode) {
		return false, errors.New("Invalid signature length")
	}
	s := bytes2big(signature[:pub.mode])
	r := bytes2big(signature[pub.mode:])
	if r.Cmp(zero) <= 0 || r.Cmp(pub.c.Q) >= 0 || s.Cmp(zero) <= 0 || s.Cmp(pub.c.Q) >= 0 {
		return false, nil
	}
	e := bytes2big(digest)
	e.Mod(e, pub.c.Q)
	if e.Cmp(zero) == 0 {
		e = big.NewInt(1)
	}
	v := big.NewInt(0)
	v.ModInverse(e, pub.c.Q)
	z1 := big.NewInt(0)
	z2 := big.NewInt(0)
	z1.Mul(s, v)
	z1.Mod(z1, pub.c.Q)
	z2.Mul(r, v)
	z2.Mod(z2, pub.c.Q)
	z2.Sub(pub.c.Q, z2)
	p1x, p1y, err := pub.c.Exp(z1, pub.c.Bx, pub.c.By)
	if err != nil {
		return false, err
	}
	q1x, q1y, err := pub.c.Exp(z2, pub.x, pub.y)
	if err != nil {
		return false, err
	}
	lm := big.NewInt(0)
	lm.Sub(q1x, p1x)
	if lm.Cmp(zero) < 0 {
		lm.Add(lm, pub.c.P)
	}
	lm.ModInverse(lm, pub.c.P)
	z1.Sub(q1y, p1y)
	lm.Mul(lm, z1)
	lm.Mod(lm, pub.c.P)
	lm.Mul(lm, lm)
	lm.Mod(lm, pub.c.P)
	lm.Sub(lm, p1x)
	lm.Sub(lm, q1x)
	lm.Mod(lm, pub.c.P)
	if lm.Cmp(zero) < 0 {
		lm.Add(lm, pub.c.P)
	}
	lm.Mod(lm, pub.c.Q)
	return lm.Cmp(r) == 0, nil
}
