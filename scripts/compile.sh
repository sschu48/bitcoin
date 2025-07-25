#!/bin/bash
input="$1"
if [[ ! "$input" =~ \.c$ ]]; then
    input="$input.c"
fi
filename=$(basename "$input" .c)
clang -fsanitize=address -g -I$(brew --prefix openssl)/include -Iinclude -L$(brew --prefix openssl)/lib -lssl -lcrypto "$input" src/transaction.c src/block.c src/balance.c -o "bin/$filename"