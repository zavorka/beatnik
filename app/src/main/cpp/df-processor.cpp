#include <jni.h>
#include <string>
#include <vector>

#include "beatnik/FFT_rolling.hpp"
#include "tracker/CSD_detection_function.hpp"
#include "tracker/Tracker.hpp"
#include "log.h"

#define TAG "beatnik"



static reBass::FFT_rolling* fft;
static reBass::CSD_detection_function* detection_function;

static std::vector<float>* input_buffer;

extern "C"
void
Java_re_bass_beatnik_audio_NativeDFProcessor_init(
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
void
Java_re_bass_beatnik_audio_NativeDFProcessor_processAudio(
    JNIEnv* env,
    jobject object, /* this */
    jfloatArray inputArray,
    jdoubleArray outputArray
) {
    auto windows_count = env->GetArrayLength(outputArray);
    auto step_size = input_buffer->size();
    auto output = env->GetDoubleArrayElements(outputArray, nullptr);

    for (unsigned short i = 0; i < windows_count; i++) {
        env->GetFloatArrayRegion(
                inputArray,
                i * step_size,
                step_size,
                input_buffer->data()
        );

        auto fft_buffer = fft->compute_fft(*input_buffer);
        output[i] = detection_function->process_frequency_domain(fft_buffer);

        if (std::isnan(output[i])) {
            LOGE("DF value is NaN!");
        }

        output[i] /= reBass::CSD_detection_function::DF_OUTPUT_VALUE_MULTIPLIER;
    }

    env->ReleaseDoubleArrayElements(outputArray, output, 0);
}

extern "C"
void
Java_re_bass_beatnik_audio_NativeDFProcessor_calculateMagnitudes(
        JNIEnv* env,
        jobject object, /* this */
        jfloatArray output
) {
    auto magnitudes = fft->calculate_magnitudes();
    env->SetFloatArrayRegion(
            output,
            0,
            magnitudes.size(),
            magnitudes.data()
    );
}

extern "C"
void
Java_re_bass_beatnik_audio_NativeDFProcessor_getFrequencyData(
        JNIEnv* env,
        jobject object, /* this */
        jfloatArray output
) {
    auto frequencyData = fft->get_frequency_domain_data();
    env->SetFloatArrayRegion(
            output,
            0,
            frequencyData.size() * 2,
            reinterpret_cast<const float *>(frequencyData.data())
    );
}