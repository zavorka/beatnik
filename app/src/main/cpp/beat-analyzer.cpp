#include <jni.h>
#include <android/log.h>

#include "tracker/Beat_analyzer.hpp"

#define TAG "BeatAnalyzer"

static reBass::Beat_analyzer* analyzer = nullptr;

extern "C"
void
Java_re_bass_beatnik_BeatAnalyzer_init(
        JNIEnv* env,
        jobject object, /* this */
        jint sampleRate,
        jint stepSize,
        jint windowSize
) {
    analyzer = new reBass::Beat_analyzer(
            (unsigned int) sampleRate,
            (size_t) stepSize,
            (size_t) windowSize
    );
}

extern "C"
void
Java_re_bass_beatnik_BeatAnalyzer_enqueueDFValue(
        JNIEnv* env,
        jobject object, /* this */
        jdouble dfValue
) {
    if (analyzer == nullptr) {
        return;
    }
    analyzer->enqueue_df_value(dfValue);
}

extern "C"
float
Java_re_bass_beatnik_BeatAnalyzer_getBPM(
        JNIEnv* env,
        jobject object /* this */
) {
    if (analyzer == nullptr) {
        return -1.f;
    }
    return analyzer->get_bpm();
}

extern "C"
void
Java_re_bass_beatnik_BeatAnalyzer_clearData(
        JNIEnv* env,
        jobject object /* this */
) {
    if (analyzer == nullptr) {
        return;
    }
    analyzer->clear_data();
}