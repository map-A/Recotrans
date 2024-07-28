cmake_minimum_required(VERSION 3.26)
project(translate)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(Qt5_DIR C:\\Qt\\Qt5.14.1\\5.14.1)
set(RTAUDIO_DIR "${PROJECT_SOURCE_DIR}/submodules/rtaudio")
set(WHISPER_DIR "${PROJECT_SOURCE_DIR}/submodules/whisper.cpp")

add_subdirectory(submodules/rtaudio)
add_subdirectory(submodules/whisper.cpp)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
include_directories("${Qt5_DIR}/msvc2017_64/include/QtWidgets/"
        "${Qt5_DIR}/msvc2017_64/include/QtGui"
        "${Qt5_DIR}/msvc2017_64/include/"
        "${WHISPER_DIR}/include"
        "${WHISPER_DIR}/ggml/include"
        "${RTAUDIO_DIR}/"
        )

link_directories("${Qt5_DIR}/msvc2017_64/lib"
        "${PROJECT_SOURCE_DIR}/cmake-build-debug/submodules/rtaudio/")

add_executable(testa
        main.cpp
        floatingWindow.cpp
        floatingWindow.h
        stream.h
        stream.cpp
        audio.h
        audio.cpp
        stringgenerator.h
        stringgenerator.cpp
)
target_link_libraries(testa PRIVATE Qt5::Widgets Qt5::Core whisper rtaudiod ${CMAKE_THREAD_LIBS_INIT})



