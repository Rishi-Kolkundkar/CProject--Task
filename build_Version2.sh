#!/bin/bash

echo "Compiling C++ SHA-256 implementation..."
# Compile your SHA-256 code into an object file
g++ -std=c++11 -c -o SHA_Hash.o SHA_Hash.cpp

echo "Compiling C++ wrapper..."
# Compile the C-to-C++ bridge
g++ -std=c++11 -c -o sha256_wrapper.o sha256_wrapper.cpp

echo "Compiling user.c..."
# Compile the C user code
gcc -c -o user.o user.c

echo "Compiling tasks.c..."
# Compile the C tasks code
gcc -c -o tasks.o tasks.c

echo "Linking all object files into the final executable..."
# Use g++ for the final linking step to automatically include the C++ standard library
g++ -o task_manager user.o tasks.o SHA_Hash.o sha256_wrapper.o

echo "Build finished! Run with ./task_manager"
