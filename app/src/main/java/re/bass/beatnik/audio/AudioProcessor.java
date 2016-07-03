package re.bass.beatnik.audio;

/**
 * Created by curly on 24/06/2016.
 */

public interface AudioProcessor extends AudioInput.AudioListener {
    void addOnProcessorOutputListener(OnProcessorOutputListener listener);
    void removeOnProcessorOutputListener(OnProcessorOutputListener listener);

    int getWindowSize();
    int getFFTSize();

    interface OnProcessorOutputListener {
        void onProcessorOutput(
                double output,
                float[] frequencyDomain,
                float[] magnitudes
        );
    }
}
