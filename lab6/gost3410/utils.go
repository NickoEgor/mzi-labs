package gost3410

import (
	"math/big"
)

func bytes2big(d []byte) *big.Int {
	n := big.NewInt(0)
	n.SetBytes(d)
	return n
}

func reverse(d []byte) {
	for i, j := 0, len(d)-1; i < j; i, j = i+1, j-1 {
		d[i], d[j] = d[j], d[i]
	}
}

func pad(d []byte, size int) []byte {
	return append(make([]byte, size-len(d)), d...)
}
