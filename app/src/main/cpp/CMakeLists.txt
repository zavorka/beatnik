cmake_minimum_required(VERSION 3.6.0)

if (NOT DEFINED ANDROID_NDK)
    set(ANDROID_NDK /Users/curly/Library/Android/sdk/ndk-bundle)
endif()

##########################################################
# build cpufeatures as a static lib
include_directories(./ndk_helper
                    ${ANDROID_NDK}/sources/android/cpufeatures)
add_library(cpufeatures  STATIC
            ${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)

##########################################################

set(BOOST_DIR ../../../external/boost)
include_directories(${BOOST_DIR}/include)

set(SRC
        external/kiss_fft/_kiss_fft_guts.h
        external/kiss_fft/kiss_fft.h
        external/kiss_fft/kiss_fft.c
        external/kiss_fft/kiss_fftr.h
        external/kiss_fft/kiss_fftr.c

        beatnik/Window.hpp
        beatnik/FFT_rolling.hpp
        beatnik/FFT_rolling.cpp

        tracker/Beat_analyzer.hpp
        tracker/Beat_analyzer.cpp
        tracker/CSD_detection_function.hpp
        tracker/CSD_detection_function.cpp
        tracker/math_utilities.hpp
        tracker/math_utilities.cpp
        tracker/Tracker.hpp
        tracker/Tracker.cpp

        beat-analyzer.cpp
        cpu-features.cpp
        df-processor.cpp
        log.h
)

add_library(beatnik SHARED ${SRC})
target_link_libraries(beatnik android log cpufeatures)