#include <jni.h>
#include <string>
#include <android/log.h>
#include <time.h>
#include <sys/types.h>

#include "beatnik/FFT_rolling.hpp"
#include "tracker/CSD_detection_function.hpp"
#include "tracker/Tracker.hpp"

#define TAG "beatnik"

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , TAG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO   , TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN   , TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , TAG,__VA_ARGS__)

static reBass::FFT_rolling* fft;
static reBass::CSD_detection_function* detection_function;

static size_t called = 0;

extern "C"
void
Java_re_bass_beatnik_audio_DFAudioProcessor_init(
        JNIEnv* env,
        jobject object, /* this */
        jint sampleRate,
        jint stepSize,
        jint windowSize
) {
    fft = new reBass::FFT_rolling((size_t) windowSize);
    detection_function = new reBass::CSD_detection_function(
            (size_t) windowSize,
            (size_t) stepSize
    );
}

extern "C"
double
Java_re_bass_beatnik_audio_DFAudioProcessor_processAudio(
    JNIEnv* env,
    jobject object, /* this */
    jobject input,
    jint offset,
    jint size
) {
    auto buffer = ((const float* const) env->GetDirectBufferAddress(input)) + offset;
    return detection_function->processFrequencyDomain(
            fft->compute_fft(
                    buffer,
                    (size_t) size
            )
    );
}