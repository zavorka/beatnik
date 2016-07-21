#include <jni.h>
#include <string>
#include <iterator>
#include <array>
#include <vector>
#include <boost/circular_buffer.hpp>

#include "tracker/fft/FFT_rolling.hpp"
#include "tracker/Broadband_DF.hpp"
#include "tracker/constants.hpp"
#include "tracker/vector_operations/multiplication.h"
#include "log.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

using namespace reBass;
using namespace reBass::constants;

static FFT_rolling<FFT_WINDOW_LENGTH>* fft;
static Broadband_DF<FFT_LENGTH>* detection_function;

static std::array<float, FFT_STEP_SIZE>* input_buffer;
static std::array<short, FFT_STEP_SIZE>* short_buffer;

static boost::circular_buffer<float>* df_plot_circular_buffer;
static float* df_plot_buffer;

static float* fft_plot_buffer;
static unsigned int fft_plot_buffer_length;

extern "C"
void
Java_re_bass_beatnik_audio_NativeDFProcessor_init(
        JNIEnv* env,
        jobject object /* this */
) {
    input_buffer = new std::array<float, FFT_STEP_SIZE>();
    short_buffer = new std::array<short, FFT_STEP_SIZE>;
    fft = new reBass::FFT_rolling<FFT_WINDOW_LENGTH>;
    detection_function = new reBass::Broadband_DF<FFT_LENGTH>;
}

extern "C"
void
Java_re_bass_beatnik_audio_NativeDFProcessor_dealloc(
        JNIEnv* env,
        jobject object /* this */
) {
    if (fft != nullptr) {
        delete fft;
    }
    if (detection_function != nullptr) {
        delete detection_function;
    }
    if (input_buffer != nullptr) {
        delete input_buffer;
    }
    if (short_buffer != nullptr) {
        delete short_buffer;
    }
    if (df_plot_circular_buffer != nullptr) {
        delete df_plot_circular_buffer;
    }
    df_plot_buffer = nullptr;
    fft_plot_buffer = nullptr;
}


extern "C"
void
Java_re_bass_beatnik_audio_NativeDFProcessor_processAudio(
    JNIEnv* env,
    jobject object, /* this */
    jshortArray inputArray,
    jfloatArray outputArray
) {
    auto windows_count = env->GetArrayLength(outputArray);
    auto step_size = input_buffer->size();
    auto output = env->GetFloatArrayElements(outputArray, nullptr);
    float average = 0.0f;

    for (unsigned short i = 0; i < windows_count; i++) {
        env->GetShortArrayRegion(
                inputArray,
                i * (jsize) step_size,
                (jsize) step_size,
                short_buffer->data()
        );

        std::transform(
                std::cbegin(*short_buffer),
                std::cend(*short_buffer),
                std::begin(*input_buffer),
                [] (short sample) {
                    return (float) sample
                           / (float) std::numeric_limits<short>::max();
                }
        );

        fft->compute_fft(std::cbegin(*input_buffer), std::cend(*input_buffer));


        auto magnitudes = fft->calculate_magnitudes();

        output[i] = detection_function->process_magnitudes(magnitudes);

        output[i] /= DF_OUTPUT_VALUE_MULTIPLIER;
        average += (float) output[i] / windows_count * 2;
    }

    env->ReleaseFloatArrayElements(outputArray, output, 0);

    if (df_plot_circular_buffer != nullptr
            || df_plot_buffer != nullptr) {
        df_plot_circular_buffer->push_back(average);
        std::copy(
            std::cbegin(*df_plot_circular_buffer),
            std::cend(*df_plot_circular_buffer),
            df_plot_buffer
        );
    }
    if (fft_plot_buffer != nullptr) {
        auto magnitudes = fft->calculate_magnitudes();
        auto length = fft_plot_buffer_length;
        if (magnitudes.size() < length) {
            length = (unsigned int) magnitudes.size();
        }

        std::copy(
            std::cbegin(magnitudes),
            std::cbegin(magnitudes) + length,
            fft_plot_buffer
        );
    }
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
            (jsize) magnitudes.size(),
            magnitudes.data()
    );
}

extern "C"
void
Java_re_bass_beatnik_audio_NativeDFProcessor_attachDFPlotBuffer(
        JNIEnv* env,
        jobject object, /* this */
        jobject bufferObject,
        jint length
) {
    df_plot_buffer = reinterpret_cast<float*>(env->GetDirectBufferAddress(bufferObject));
    df_plot_circular_buffer = new boost::circular_buffer<float>((unsigned int) length);
    df_plot_circular_buffer->insert(
            std::begin(*df_plot_circular_buffer),
            df_plot_circular_buffer->capacity(),
            0.0f
    );
}

extern "C"
void
Java_re_bass_beatnik_audio_NativeDFProcessor_attachFFTPlotBuffer(
        JNIEnv* env,
        jobject object, /* this */
        jobject bufferObject,
        jint length
) {
    fft_plot_buffer = reinterpret_cast<float*>(env->GetDirectBufferAddress(bufferObject));
    fft_plot_buffer_length = (unsigned int) length;
}

extern "C"
void
Java_re_bass_beatnik_audio_NativeDFProcessor_detachDFPlotBuffer(
        JNIEnv* env,
        jobject object
) {
    df_plot_buffer = nullptr;
}

extern "C"
void
Java_re_bass_beatnik_audio_NativeDFProcessor_detachFFTPlotBuffer(
        JNIEnv* env,
        jobject object
) {
    fft_plot_buffer = nullptr;
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
            (jsize) frequencyData.size() * 2,
            reinterpret_cast<const float *>(frequencyData.data())
    );
}
#pragma clang diagnostic pop