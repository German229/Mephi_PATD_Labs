cmake_minimum_required(VERSION 3.30)
project(Mephi_PATD_Labs C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)

# Лабораторная 1 — C
add_executable(lab1
        Semester_2_Lab_1/main.c
        Semester_2_Lab_1/matrix_4.c
)
set_target_properties(lab1 PROPERTIES LINKER_LANGUAGE C)

# Лабораторная 2 — C++
add_executable(lab2
        Semester_2_Lab_2/main.cpp
        Semester_2_Lab_2/Tests.cpp
        Semester_2_Lab_2/dynamic_array.h
        Semester_2_Lab_2/Lists.h
        Semester_2_Lab_2/sequence.h
        Semester_2_Lab_2/Tests.h
)

# Лабораторная 3 — C++
add_executable(lab3
        Semester_2_Lab_3/main.cpp
        Semester_2_Lab_3/Tests.cpp
        Semester_2_Lab_3/Matrix_MAS.h
        Semester_2_Lab_3/dynamic_array.h
        Semester_2_Lab_3/Lists.h
        Semester_2_Lab_3/sequence.h
)

# Лабораторная 4 — C++
add_executable(lab4
        Semester_2_Lab_4/main_res.cpp
        Semester_2_Lab_4/Tree.h
        Semester_2_Lab_4/Menu.h
        Semester_2_Lab_4/Person.h
        Semester_2_Lab_4/Student.h
        Semester_2_Lab_4/Teacher.h
)
