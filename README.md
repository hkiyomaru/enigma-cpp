# Enigma Simulator


## What is Enigma

Enigma is one of the substituion cipher machines.

When you encrypt a message, you set a secret key and input it.
The output is the ciphertext.

To get the plaintext, input the output in the same way with the key you set.

## Instruction

Clone this repository.

```
$ git clone https://github.com/kiyomaro927/EnigmaSimulator.git
```

I used the g++ as a compiler.
If you have Ubuntu, the installation is like this.

```
$ sudo apt-get update
$ sudo apt-get install g++
```

After that, you need to get Boost c++ libraries.

```
$ sudo apt-get libboost-dev
```

## Build

```
$ g++ -std=c++11 enigma.cpp -o enigma
```

## Run

```
$ ./enigma TARGET_STRING
```

This script has some options.
You can show the all options and their overview with the command bellow.

```
$ ./enigma -h
```
