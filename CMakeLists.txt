cmake_minimum_required(VERSION 3.20)

project(media_foundation)

add_definitions(-DUNICODE)

add_subdirectory(list_video_device)
add_subdirectory(video_capture)
add_subdirectory(video_capture_ability)