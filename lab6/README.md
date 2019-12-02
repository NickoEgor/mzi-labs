# Lab 6

Before use it you need to compile hash function by Makefile_hash
[-s] == 256bit mode
./genkey [-s] file_name

Generate 512bit or 256bit private key
./conv [-s] priv_key_file pub_key_file

Generate public key and write to file
./sign [-s] priv_key_file message sign_file

Message gets his digital signature
Digital signtature written in sign_file
./verify [-s] public_key_file message sign_file
Confirm or deny digital sign of message
