package gost3410

import (
	"math/big"
)

func NewUKM(raw []byte) *big.Int {
	t := make([]byte, len(raw))
	copy(t, raw)
	reverse(t)
	return bytes2big(t)
}
