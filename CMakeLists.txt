cmake_minimum_required(VERSION 3.20)

project(media_foundation)

add_definitions(-DUNICODE)

file(GLOB SRCS 
./*.cpp)

add_executable(${PROJECT_NAME} ${SRCS})