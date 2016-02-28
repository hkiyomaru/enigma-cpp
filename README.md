#Enigma Simulator


##What is Enigma

Enigma is one of the substituion cipher machines.

When you encrypt a message,

you set a secret key and input that.

The output is ciphertext.

To get plaintext,

input the output in the same way with the key you set.


##Instruction

Download this repository.

```
$ git clone https://github.com/kiyomaro927/EnigmaSimulator.git
```

To compile this program, I used the g++.

If you have Ubuntu,

the installation is like this.

```
$ sudo apt-get update
$ sudo apt-get install g++
```

After that, you have to get Boost c++ Libraries.

```
$ sudo apt-get libboost-dev
```

##Build

```
$ g++ -std=c++11 enigma.cpp -o enigma
```

##Run

```
$ ./enigma TARGET_STRING
```

This has some options.

When you'd like to show all options and their overview,

```
$ ./enigma -h
```
