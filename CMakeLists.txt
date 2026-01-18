cmake_minimum_required(VERSION 3.20)

project(FileCompressor)

add_executable(Compressor 
    files.cpp
    main.cpp
)

target_include_directories(Compressor
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)