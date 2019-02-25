#!/bin/sh

rm -rf ./src/*.pb.h
rm -rf ./src/*.pb.cc

protoc -I=./proto --cpp_out=./proto test.proto

cp -rf ./proto/*.pb.h ./src
cp -rf ./proto/*.pb.cc ./src
