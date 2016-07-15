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
double
Java_re_bass_beatnik_audio_BTrack_processDFSample(
        JNIEnv* env,
        jobject object, /* this */
        jdouble dfValue
) {
    return bTrack->process_DF_sample(dfValue * multiplier);
}