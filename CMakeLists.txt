cmake_minimum_required(VERSION 4.3)
project(Sistemas_Operacionais C)

set(CMAKE_C_STANDARD 11)

include_directories(Atividade01)

add_executable(Sistemas_Operacionais
        Atividade01/queue.c
        Atividade01/queue.h
        Atividade01/testafila.c)
