cmake_minimum_required(VERSION 3.31)
project(Mephi_PATD_Labs C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)

# Лабораторная 1 семестр 2 — C
add_executable(lab1
        Semester_2_Lab_1/main.c
        Semester_2_Lab_1/matrix_4.c
)
set_target_properties(lab1 PROPERTIES LINKER_LANGUAGE C)

# Лабораторная 2 семестр 2 — C++
add_executable(lab2
        Semester_2_Lab_2/main.cpp
        Semester_2_Lab_2/Tests.cpp
)

# Лабораторная 3 семестр 2 — C++
add_executable(lab3
        Semester_2_Lab_3/main.cpp
        Semester_2_Lab_3/Tests.cpp
)

# Лабораторная 4 семестр 2 — C++
add_executable(lab4
        Semester_2_Lab_4/main_res.cpp
)

# Лабораторная 5 семестр 2 — C++
add_executable(lab5
        Semester_2_Lab_5/main.cpp
        Semester_2_Lab_5/Deque.cpp
)

# Домашка 1 семестр 3 — C++
add_executable(hw1
        Semester_3_HW_1/main.cpp
        Semester_3_HW_1/UnqPtr.hpp
        Semester_3_HW_1/ShrdPtr.hpp
)