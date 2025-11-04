cmake_minimum_required(VERSION 3.10.0)

project(PP1)

include_directories(include)

add_executable(${PROJECT_NAME}
src/main.c
src/functions.c
)