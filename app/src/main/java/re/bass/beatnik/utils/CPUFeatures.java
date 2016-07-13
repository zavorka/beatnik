package re.bass.beatnik.utils;

import android.util.Log;

/**
 * Created by curly on 7/13/16.
 */

public class CPUFeatures {
    private static final String TAG = "CPUFeatures";

    public static native boolean hasVFPv2();
    public static native boolean hasVFPv3();
    public static native boolean hasVFPD32();
    public static native boolean hasARMv7();
    public static native boolean hasNEON();
    public static native boolean hasVfpFP16();
    public static native boolean hasVfpFMA();
    public static native boolean hasNeonFMA();
    public static native boolean hasIdivARM();
    public static native boolean hasIdivThumb2();
    public static native boolean hasIWMMXT();
    public static native boolean hasLdrexStrex();

    public static void logFeatures() {

        Log.v(TAG, "Has VFPv2: " + hasVFPv2());
        Log.v(TAG, "Has VFPv3: " + hasVFPv3());
        Log.v(TAG, "Has VFP_D32: " + hasVFPD32());
        Log.v(TAG, "Has ARMv7: " + hasARMv7());
        Log.v(TAG, "Has NEON: " + hasNEON());
        Log.v(TAG, "Has VFP_FP16: " + hasVfpFP16());
        Log.v(TAG, "Has VFP_FMA: " + hasVfpFMA());
        Log.v(TAG, "Has NEON_FMA: " + hasNeonFMA());
        Log.v(TAG, "Has IDIV_ARM: " + hasIdivARM());
        Log.v(TAG, "Has IDIV_THUMB2: " + hasIdivThumb2());
        Log.v(TAG, "Has iWMMXt: " + hasIWMMXT());
        Log.v(TAG, "Has LDREX_STREX: " + hasLdrexStrex());
    }
}
