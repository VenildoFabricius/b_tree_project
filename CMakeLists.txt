cmake_minimum_required(VERSION 3.28)
project(Trabalho_2 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(Trabalho_2
        btree.c
        btree.h
        gera_dados.c
        gera_dados.h
        main.c
        fcs_main.c
        fcs_main.h)
