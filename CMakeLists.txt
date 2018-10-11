cmake_minimum_required(VERSION 2.4)
project(sput_report_beispiel C)

set(CMAKE_C_STANDARD 90)

link_libraries(xml2)

add_executable(sput_report_beispiel main.c main.c)