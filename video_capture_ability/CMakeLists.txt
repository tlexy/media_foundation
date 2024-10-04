cmake_minimum_required(VERSION 3.20)

project(video_capture_ability)

add_definitions(-DUNICODE)

file(GLOB SRCS 
./*.cpp)

add_executable(${PROJECT_NAME} ${SRCS})