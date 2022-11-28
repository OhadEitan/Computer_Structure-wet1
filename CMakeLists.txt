cmake_minimum_required(VERSION 3.21)
project(wet1)

set(CMAKE_CXX_STANDARD 14)

add_executable(
                bp_main.cpp
                bp_api.h
                bp.cpp
                FullTableGhGt.h
                FullTableGhGt.cpp
                FullTableLhGt.cpp
                FullTableLhGt.h
                FullTableLhLt.cpp
                FullTableLhLt.h
                Row.cpp
                Row.h
                RowLh.cpp
                RowLh.h
                RowLhLt.cpp
                RowLhLt.h
                example1.trc
                example1.out
        RowGhLt.cpp
        RowGhLt.h
        FullTableGHLT.cpp
        FullTableGHLT.h)