cmake_minimum_required(VERSION 3.26)
project(hnsw_bptree_filter)

set(CMAKE_CXX_STANDARD 14)

add_executable(hnsw_bptree_filter
        BPlusTree.cpp)
