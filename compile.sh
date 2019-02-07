#!/bin/bash
exec clang++ -std=c++14 -Wall -Ofast -mtune=native -march=native main.cpp
