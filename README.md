# chacha20

A toy implementation of ChaCha20.
Currently written in C, might try Rust later.

## What is ChaCha20

ChaCha20 is a stream cipher created by Daniel J. Bernstein.
Its functioning relies on a secure Pseudo-Random Generator (PRG).

This PRG is composed of main components:

- **Padding function**: takes as input a 256-bit seed, a 64-bit counter and a 64-bit nonce, return a 512-bit block.
- **Fixed public permutation**: take as input the 512-bit block and permutes it into different positions. 
