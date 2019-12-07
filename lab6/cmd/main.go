package main

import (
	"bufio"
	"crypto/rand"
	"fmt"
	"io/ioutil"
	"os"

	gost "mzi-lab6/gost3410"
)

func check(err error) {
	if err != nil {
		fmt.Println(err)
		os.Exit(1)
	}
}

func main() {
	c, _ := gost.NewCurveFromParams(gost.CurveParamsGostR34102012TC26ParamSetA)

	reader := bufio.NewReader(os.Stdin)
	fmt.Print("Enter private key: ")
	rawKey, _ := reader.ReadString('\n')

	data := make([]byte, 64)
	// rand.Read(data)
	copy(data, rawKey)

	prv, err := gost.NewPrivateKey(c, gost.Mode2012, data)
	check(err)
	fmt.Println("Private key:", prv.Raw())

	pub, err := prv.PublicKey()
	check(err)
	fmt.Println("Public key:", pub.Raw())

	digest, err := ioutil.ReadFile("data.txt")
	fmt.Println(string(digest))
	// digest := make([]byte, 1024)
	// rand.Read(digest)

	sign, err := prv.SignDigest(digest[:], rand.Reader)
	check(err)

	fmt.Println("Signature:", sign)

	valid, err := pub.VerifyDigest(digest[:], sign)
	check(err)

	if valid {
		fmt.Println("Successful verification")
	} else {
		fmt.Println("Verification failed")
	}
}
