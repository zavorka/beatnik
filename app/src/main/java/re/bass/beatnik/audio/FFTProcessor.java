package re.bass.beatnik.audio;

/**
 * Created by curly on 05/07/16.
 */

public interface FFTProcessor {

    int getWindowSize();
    int getFFTSize();

    float[] getFFTBuffer();
    float[] getMagnitudes();

    void addOnNewFFTDataListener(OnNewFFTDataListener listener);
    void removeOnNewFFTDataListener(OnNewFFTDataListener listener);

    interface OnNewFFTDataListener {
        void onNewFFTData(
                FFTProcessor sender
        );
    }
}
