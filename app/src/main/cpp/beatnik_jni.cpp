#include "missing_std.hpp"
#include <cassert>
#include <jni.h>
#include <beatnik/Beatnik.hpp>

using namespace reBass;


static constexpr unsigned SAMPLE_RATE = 44100;
static Beatnik<> beatnik(SAMPLE_RATE);
static gsl::span<float> fft_plot_buffer;

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

    assert(input_buffer.size() % beatnik.fft_step == 0);
    auto new_tempo = false;
    for (auto i = 0; i < input_buffer.size(); i += beatnik.fft_step) {
        new_tempo = new_tempo ||
                beatnik.process(input_buffer.subspan<beatnik.fft_step>(i));
    }

    auto magnitudes = beatnik.get_magnitudes();
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
jfloat Java_re_bass_beatnik_audio_Beatnik_getCurrentTempo(JNIEnv*, jobject)
{
    return beatnik.get_current_tempo();
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
    return beatnik.fft_step;
}
