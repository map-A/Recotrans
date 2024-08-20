cmake_minimum_required(VERSION 3.26)
project(translate)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
    set(Qt5_DIR "C:/Qt/Qt5.14.1/5.14.1")
    set(Qt5_INCLUDE_DIRS "${Qt5_DIR}/msvc2017_64/include/QtWidgets/"
                         "${Qt5_DIR}/msvc2017_64/include/QtGui"
                         "${Qt5_DIR}/msvc2017_64/include/")
    set(Qt5_LIB_DIRS "${Qt5_DIR}/msvc2017_64/lib")
    set(RTAUDIO_LIB rtaudio)
elseif (APPLE)
    find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
    set(Qt5_INCLUDE_DIRS ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS})
    set(Qt5_LIB_DIRS ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES})
    set(RTAUDIO_LIB rtaudio)
elseif (UNIX AND NOT APPLE) # Linux
    find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
    set(Qt5_INCLUDE_DIRS ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS})
    set(Qt5_LIB_DIRS ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES})
    set(RTAUDIO_LIB rtaudio)
elseif (ANDROID)
    find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
    set(Qt5_INCLUDE_DIRS ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS})
    set(Qt5_LIB_DIRS ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES})
    set(RTAUDIO_LIB rtaudio)
    add_definitions(-DANDROID)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

# Submodules
set(RTAUDIO_DIR "${PROJECT_SOURCE_DIR}/submodules/rtaudio")
set(WHISPER_DIR "${PROJECT_SOURCE_DIR}/submodules/whisper.cpp")

add_subdirectory(${RTAUDIO_DIR})
add_subdirectory(${WHISPER_DIR})
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
# Set Qt options
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Include directories
include_directories(
    ${Qt5_INCLUDE_DIRS}
    "${WHISPER_DIR}/include"
    "${WHISPER_DIR}/ggml/include"
    "${RTAUDIO_DIR}"
)

# Link directories
link_directories(${Qt5_LIB_DIRS}
                 "${PROJECT_SOURCE_DIR}/build/submodules/rtaudio/")


add_executable(lyric lyric.cpp
        LyricWidget.cpp
        LyricWidget.h
        inference.h
        inference.cpp
        stringgenerator.h
        stringgenerator.cpp
        audio.h
        audio.cpp

        )
target_link_libraries(lyric PRIVATE Qt5::Widgets Qt5::Core whisper ${RTAUDIO_LIB} ${CMAKE_THREAD_LIBS_INIT})



