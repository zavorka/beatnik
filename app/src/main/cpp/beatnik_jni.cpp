#include "missing_std.hpp"
#include <cassert>
#include <jni.h>
#include <beatnik/Beatnik.hpp>

using namespace reBass;

static constexpr int FFT_SIZE = 1024;
static constexpr int FFT_STEP = 128;
static constexpr int ODF_SIZE = 512;
static constexpr int ODF_STEP = 128;
static constexpr int SAMPLE_RATE = 44100;

using Beatnik_t = Beatnik<float, FFT_SIZE, FFT_STEP, ODF_SIZE, ODF_STEP>;

static Beatnik_t beatnik(SAMPLE_RATE);
static gsl::span<float> fft_plot_buffer;
static std::array<float, FFT_STEP> float_buffer;

template <typename T>
static
gsl::span<T> span_from_direct_buffer(
    JNIEnv* env,
    jobject bufferObject
) noexcept {
    auto const buffer_ptr = reinterpret_cast<T*>(
            env->GetDirectBufferAddress(bufferObject)
    );
    auto const buffer_size = static_cast<std::size_t>(
            env->GetDirectBufferCapacity(bufferObject)
    );
    return gsl::span<T>(buffer_ptr, buffer_size);
}

extern "C"
bool Java_re_bass_beatnik_audio_Beatnik_process(
    JNIEnv* env,
    jobject,
    jobject bufferObject
) noexcept {
    auto input_buffer = span_from_direct_buffer<short const>(env, bufferObject);

    assert(std::size(input_buffer) % beatnik.fft_step == 0);
    auto new_tempo = false;
    for (auto i = 0; i < std::size(input_buffer); i += FFT_STEP) {
        auto step = input_buffer.subspan<FFT_STEP>(i);
        std::transform(
            std::cbegin(step),
            std::cend(step),
            std::begin(float_buffer),
            [] (short sample) {
                return
                    static_cast<float>(sample)
                    / static_cast<float>(std::numeric_limits<short>::max());
            }
        );

        new_tempo = new_tempo || beatnik.process(float_buffer);
    }

    auto magnitudes = beatnik.get_fft_magnitudes();
    std::copy(
        std::cbegin(magnitudes),
        std::cbegin(magnitudes)
        + std::min(magnitudes.size(), fft_plot_buffer.size()),
        std::begin(fft_plot_buffer)
    );

    return new_tempo;
}

extern "C"
void Java_re_bass_beatnik_audio_Beatnik_clear(JNIEnv*, jobject)
noexcept {
    fft_plot_buffer = {};
    beatnik.clear();
}

extern "C"
jfloat Java_re_bass_beatnik_audio_Beatnik_estimateTempo(JNIEnv*, jobject)
{
    return beatnik.estimate_tempo();
}

extern "C"
void Java_re_bass_beatnik_audio_Beatnik_attachFFTPlotBuffer(
    JNIEnv* env,
    jobject,
    jobject bufferObject
) noexcept {
    fft_plot_buffer = span_from_direct_buffer<float>(env, bufferObject);
}

extern "C"
jint Java_re_bass_beatnik_audio_Beatnik_getRequiredSampleRate(JNIEnv*, jobject)
{
    return SAMPLE_RATE;
}

extern "C"
jint Java_re_bass_beatnik_audio_Beatnik_getRequiredStepSize(JNIEnv*, jobject)
{
    return FFT_STEP;
}
