cmake_minimum_required(VERSION 3.10)
project(SystemProgrammingLab C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(SystemProgrammingLab
        Assembler.c
        AuxFunc.c
        data.h
        DataTypes.c
        FirstPass.c
        LineAnalysis.c
        SecondPass.c)
