#include <jni.h>
#include <string>
#include <vector>
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

static std::vector<float>* input_buffer;

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
    input_buffer = new vector<float>((size_t) stepSize);
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
    jfloatArray inputArray,
    jint offset,
    jint size,
    jfloatArray fftBufferArray,
    jfloatArray magnitudesArray
) {
    env->GetFloatArrayRegion(inputArray, offset, size, input_buffer->data());
    auto fft_buffer = fft->compute_fft(*input_buffer);

    env->SetFloatArrayRegion(
            fftBufferArray,
            0,
            fft_buffer.size() * 2,
            reinterpret_cast<const float*>(fft_buffer.data())
    );

    auto magnitudes = fft->get_magnitudes();
    env->SetFloatArrayRegion(
            magnitudesArray,
            0,
            magnitudes.size(),
            magnitudes.data()
    );
    return detection_function->process_frequency_domain(fft_buffer);
}