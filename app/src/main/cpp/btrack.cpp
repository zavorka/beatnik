#include <jni.h>

#include "tracker/BTrack.hpp"
#include "tracker/Broadband_DF.hpp"
#include "tracker/constants.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

static reBass::BTrack* bTrack;
static auto multiplier = reBass::constants::DF_OUTPUT_VALUE_MULTIPLIER;

extern "C"
void
Java_re_bass_beatnik_audio_BTrack_init(
        JNIEnv* env,
        jobject object /* this */
) {
    bTrack = new reBass::BTrack(
            reBass::constants::SAMPLE_RATE,
            reBass::constants::FFT_STEP_SIZE
    );
}

extern "C"
float
Java_re_bass_beatnik_audio_BTrack_getBpm()
{
    return bTrack->get_BPM();
}

extern "C"
bool
Java_re_bass_beatnik_audio_BTrack_processDFSample(
        JNIEnv* env,
        jobject object, /* this */
        jfloat dfValue
) {
    return bTrack->process_DF_sample(dfValue * multiplier);
}

extern "C"
bool
Java_re_bass_beatnik_audio_BTrack_processDFSamples(
        JNIEnv* env,
        jobject object, /* this */
        jfloatArray valuesArray
) {
    jsize length = env->GetArrayLength(valuesArray);
    auto values = env->GetFloatArrayElements(valuesArray, nullptr);

    auto values_vector = std::vector<float>((std::size_t) length);
    for (jsize i = 0; i < length; i++) {
        values_vector[i] = (values[i] *= multiplier);
    }
    env->ReleaseFloatArrayElements(valuesArray, values, JNI_ABORT);

    return bTrack->process_DF_samples(values_vector);
}
#pragma clang diagnostic pop