cmake_minimum_required(VERSION 3.20)

project(list_video_device)

add_definitions(-DUNICODE)

file(GLOB SRCS 
./*.cpp)

add_executable(${PROJECT_NAME} ${SRCS})