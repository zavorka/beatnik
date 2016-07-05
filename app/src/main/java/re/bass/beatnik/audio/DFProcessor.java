package re.bass.beatnik.audio;

/**
 * Created by curly on 24/06/2016.
 */

public interface DFProcessor extends AudioInput.AudioListener {
    void addOnDFProcessorOutputListener(OnProcessorOutputListener listener);
    void removeOnDFProcessorOutputListener(OnProcessorOutputListener listener);

    interface OnProcessorOutputListener {
        void onProcessorOutput(
                double[] output
        );
    }
}
