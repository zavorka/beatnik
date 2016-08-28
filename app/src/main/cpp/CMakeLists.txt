cmake_minimum_required(VERSION 3.4.1)

include_directories(BEFORE
    external/beatnik/external/GSL/include
    external/beatnik/include
)

#if (${ANDROID_ABI} STREQUAL "armeabi-v7a")
#  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon")
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpu=neon")
#endif ()

set(SRC
        beatnik_jni.cpp
)
add_library(beatnik SHARED ${SRC})

target_link_libraries(beatnik android log atomic)
