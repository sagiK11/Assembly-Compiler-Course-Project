cmake_minimum_required(VERSION 3.10)
project(SystemProgrammingLab C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(SystemProgrammingLab
        Assembler.c
        "AuxiliaryFunctions .c"
        declarationsHeader.h
        KeyWords.c
        FirstPass.c
        LineAnalysis.c
        SecondPass.c ErrorPrinter.c InputOutputHandler.c SymbolTableHandler.c ParametersListHandler.c DataListHandler.c InstructionsListHandler.c ExtListHandler.c LineListHandler.c SecondPassCmdCoding.c SecondPassParametersCoding.c SecondPassSpecialActionCoding.c SecondPassRegisterCoding.c FIrstPassDataCoding.c FirstPassStringCoding.c FirstPassExternCoding.c FirstPassCmdCoding.c)
