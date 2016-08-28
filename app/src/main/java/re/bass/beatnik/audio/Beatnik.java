package re.bass.beatnik.audio;

import java.nio.ShortBuffer;
import java.nio.FloatBuffer;

/**
 * Created by curly on 8/24/16.
 */

public class Beatnik {
    public static native void clear();
    public static native boolean process(final ShortBuffer input);
    public static native float getCurrentTempo();
    public static native void attachFFTPlotBuffer(FloatBuffer plot);
    public static native int getRequiredSampleRate();
    public static native int getRequiredStepSize();
}
