#include <jni.h>

#include "tracker/CSD_detection_function.hpp"
#include "tracker/Beat_analyzer.hpp"
#include "log.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

static reBass::Beat_analyzer* analyzer = nullptr;
static auto multiplier = reBass::CSD_detection_function::DF_OUTPUT_VALUE_MULTIPLIER;

extern "C"
void
Java_re_bass_beatnik_audio_BeatAnalyzer_init(
        JNIEnv* env,
        jobject object, /* this */
        jint sampleRate,
        jint stepSize,
        jint windowSize
) {
    analyzer = new reBass::Beat_analyzer(
            (unsigned int) sampleRate,
            (unsigned int) stepSize
    );
}

extern "C"
void
Java_re_bass_beatnik_audio_BeatAnalyzer_enqueueDFValue(
        JNIEnv* env,
        jobject object, /* this */
        jdouble dfValue
) {
    if (analyzer == nullptr) {
        return;
    }
    analyzer->enqueue_df_value(dfValue * multiplier);
}

extern "C"
void
Java_re_bass_beatnik_audio_BeatAnalyzer_enqueueDFValues(
        JNIEnv* env,
        jobject object, /* this */
        jdoubleArray valuesArray
) {
    if (analyzer == nullptr) {
        return;
    }

    jsize length = env->GetArrayLength(valuesArray);
    auto values = env->GetDoubleArrayElements(valuesArray, nullptr);


    for (jsize i = 0; i < length; i++) {
        analyzer->enqueue_df_value(values[i] *= multiplier);
    }

    env->ReleaseDoubleArrayElements(valuesArray, values, JNI_ABORT);
}


extern "C"
float
Java_re_bass_beatnik_audio_BeatAnalyzer_getBPM(
        JNIEnv* env,
        jobject object /* this */
) {
    if (analyzer == nullptr) {
        return -1.f;
    }
    return analyzer->get_bpm();
}

extern "C"
float
Java_re_bass_beatnik_audio_BeatAnalyzer_getBPMWithBeats(
        JNIEnv* env,
        jobject object, /* this */
        jfloatArray beatsArray
) {
    if (analyzer == nullptr) {
        return -1.f;
    }
    return analyzer->get_bpm();
}

extern "C"
void
Java_re_bass_beatnik_audio_BeatAnalyzer_clearData(
        JNIEnv* env,
        jobject object /* this */
) {
    if (analyzer == nullptr) {
        return;
    }
    analyzer->clear_data();
}
#pragma clang diagnostic pop