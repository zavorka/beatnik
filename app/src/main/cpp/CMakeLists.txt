cmake_minimum_required(VERSION 3.4.1)

if (${ANDROID_ABI} STREQUAL "armeabi-v7a")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpu=neon")
endif ()

add_subdirectory(external/beatnik)
set(SRC
    beatnik_jni.cpp
)
add_library(beatnik_jni SHARED ${SRC})
target_link_libraries(beatnik_jni beatnik)
target_link_libraries(beatnik_jni atomic)
