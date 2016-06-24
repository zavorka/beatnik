#include <jni.h>
#include <string>
#include <android/log.h>
#include <time.h>
#include <sys/types.h>

#include "beatnik/FFT_rolling.hpp"
#include "tracker/Beat_counter.hpp"


#define TAG "beatnik"

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , TAG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO   , TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN   , TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , TAG,__VA_ARGS__)

static reBass::FFT_rolling* fft;
static reBass::Beat_counter* beat_counter;

static struct timespec now;
static size_t called = 0;

reBass::RealTime get_current_time() {
    clock_gettime(CLOCK_MONOTONIC, &now);
    return reBass::RealTime(now.tv_sec, now.tv_nsec);
}

extern "C"
void
Java_re_bass_beatnik_NativeAudioListener_init(
        JNIEnv* env,
        jobject object, /* this */
        jint sampleRate,
        jint windowSize) {
    fft = new reBass::FFT_rolling((size_t) windowSize);
    beat_counter = new reBass::Beat_counter(sampleRate);
}

extern "C"
double
Java_re_bass_beatnik_NativeAudioListener_processAudio(
    JNIEnv* env,
    jobject object, /* this */
    jobject input,
    jint offset,
    jint size
) {
    auto buffer = ((const float* const) env->GetDirectBufferAddress(input)) + offset;
    return beat_counter->process(
            fft->compute_fft(
                    buffer,
                    (size_t) size
            ),
            get_current_time()
    );
}