package re.bass.beatnik.audio;

import java.nio.FloatBuffer;

/**
 * Created by curly on 24/06/2016.
 */

public interface DFProcessor extends AudioInput.AudioListener {
    void addOnDFProcessorOutputListener(OnProcessorOutputListener listener);
    void removeOnDFProcessorOutputListener(OnProcessorOutputListener listener);

    interface OnProcessorOutputListener {
        void onProcessorOutput(
                DFProcessor sender,
                double[] output
        );
    }
}
