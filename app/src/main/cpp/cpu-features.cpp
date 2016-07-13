#include <cpu-features.h>
#include <jni.h>

bool is_ARM() {
    return android_getCpuFamily() == ANDROID_CPU_FAMILY_ARM;
}

extern "C"
jboolean
Java_re_bass_beatnik_utils_CPUFeatures_hasVFPv2(
        JNIEnv* env,
        jobject object /* this */
) {
    return android_getCpuFeatures()
           && ANDROID_CPU_ARM_FEATURE_VFPv2;
}

extern "C"
jboolean
Java_re_bass_beatnik_utils_CPUFeatures_hasARMv7(
        JNIEnv* env,
        jobject object /* this */
) {
    return android_getCpuFeatures()
           && ANDROID_CPU_ARM_FEATURE_ARMv7;
}

extern "C"
jboolean
Java_re_bass_beatnik_utils_CPUFeatures_hasVFPv3(
        JNIEnv* env,
        jobject object /* this */
) {
    return android_getCpuFeatures()
           && ANDROID_CPU_ARM_FEATURE_VFPv3;
}

extern "C"
jboolean
Java_re_bass_beatnik_utils_CPUFeatures_hasVFPD32(
        JNIEnv* env,
        jobject object /* this */
) {
    return android_getCpuFeatures()
           && ANDROID_CPU_ARM_FEATURE_VFP_D32;
}

extern "C"
jboolean
Java_re_bass_beatnik_utils_CPUFeatures_hasNEON(
        JNIEnv* env,
        jobject object /* this */
) {
    return android_getCpuFeatures()
           && ANDROID_CPU_ARM_FEATURE_NEON;
}

extern "C"
jboolean
Java_re_bass_beatnik_utils_CPUFeatures_hasVfpFP16(
        JNIEnv* env,
        jobject object /* this */
) {
    return android_getCpuFeatures()
           && ANDROID_CPU_ARM_FEATURE_VFP_FP16;
}

extern "C"
jboolean
Java_re_bass_beatnik_utils_CPUFeatures_hasVfpFMA(
        JNIEnv* env,
        jobject object /* this */
) {
    return android_getCpuFeatures()
           && ANDROID_CPU_ARM_FEATURE_VFP_FMA;
}

extern "C"
jboolean
Java_re_bass_beatnik_utils_CPUFeatures_hasNeonFMA(
        JNIEnv* env,
        jobject object /* this */
) {
    return android_getCpuFeatures()
           && ANDROID_CPU_ARM_FEATURE_NEON_FMA;
}

extern "C"
jboolean
Java_re_bass_beatnik_utils_CPUFeatures_hasIdivARM(
        JNIEnv* env,
        jobject object /* this */
) {
    return android_getCpuFeatures()
           && ANDROID_CPU_ARM_FEATURE_IDIV_ARM;
}

extern "C"
jboolean
Java_re_bass_beatnik_utils_CPUFeatures_hasIdivThumb2(
        JNIEnv* env,
        jobject object /* this */
) {
    return android_getCpuFeatures()
           && ANDROID_CPU_ARM_FEATURE_IDIV_THUMB2;
}

extern "C"
jboolean
Java_re_bass_beatnik_utils_CPUFeatures_hasIWMMXT(
        JNIEnv* env,
        jobject object /* this */
) {
    return android_getCpuFeatures()
           && ANDROID_CPU_ARM_FEATURE_iWMMXt;
}

extern "C"
jboolean
Java_re_bass_beatnik_utils_CPUFeatures_hasLdrexStrex(
        JNIEnv* env,
        jobject object /* this */
) {
    return android_getCpuFeatures()
           && ANDROID_CPU_ARM_FEATURE_LDREX_STREX;
}