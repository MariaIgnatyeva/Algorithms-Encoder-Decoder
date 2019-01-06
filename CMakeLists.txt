cmake_minimum_required(VERSION 3.7)
project(KDZ)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Huffman.h ShannonFano.h FileReaderWriter.h Encoding.h LZ77.h Decoding.h Algorithm.h)
add_executable(KDZ ${SOURCE_FILES})