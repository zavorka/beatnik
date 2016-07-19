//
// Created by Roman Beránek on 7/15/16.
//

#include <jni.h>

#include "tracker/BTrack.hpp"
#include "tracker/CSD_detection_function.hpp"

static reBass::BTrack* bTrack;
static auto multiplier = reBass::CSD_detection_function::DF_OUTPUT_VALUE_MULTIPLIER;

extern "C"
void
Java_re_bass_beatnik_audio_BTrack_init(
        JNIEnv* env,
        jobject object, /* this */
        jint sampleRate,
        jint stepSize
) {
    bTrack = new reBass::BTrack(
            (unsigned int) sampleRate,
            (unsigned int) stepSize
    );
}

extern "C"
float
Java_re_bass_beatnik_audio_BTrack_processDFSample(
        JNIEnv* env,
        jobject object, /* this */
        jfloat dfValue
) {
    return bTrack->process_DF_sample(dfValue * (float) multiplier);
}

extern "C"
float
Java_re_bass_beatnik_audio_BTrack_processDFSamples(
        JNIEnv* env,
        jobject object, /* this */
        jfloatArray valuesArray
) {
    jsize length = env->GetArrayLength(valuesArray);
    auto values = env->GetFloatArrayElements(valuesArray, nullptr);

    auto values_vector = std::vector<float>(length);
    for (jsize i = 0; i < length; i++) {
        values_vector[i] = (values[i] *= multiplier);
    }
    env->ReleaseFloatArrayElements(valuesArray, values, JNI_ABORT);

    const float bpm = bTrack->process_DF_samples(values_vector);

    return (bpm > 0.f) ? bpm : NAN;
}