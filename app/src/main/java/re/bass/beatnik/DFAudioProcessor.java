package re.bass.beatnik;

import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.FloatBuffer;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by curly on 24/06/2016.
 */

class DFAudioProcessor implements AudioInput.AudioListener, AudioProcessor
{
    private final String TAG = "DFAudioProcessor";

    private final int stepSize;
    private final int windowSize;
    private final int sampleRate;
    private boolean initialized = false;

    private int calls_count = 0;

    private List<OnProcessorOutputListener> outputListeners = new ArrayList<>();

    DFAudioProcessor(int sampleRate, int stepSize, int windowSize) {
        this.sampleRate = sampleRate;
        this.stepSize = stepSize;
        this.windowSize = windowSize;
    }

    private native void init(int sampleRate, int windowSize);
    private native double processAudio(FloatBuffer buffer, int offset, int size);

    @Override
    public void onStart() {
        init(sampleRate, windowSize);
        this.initialized = true;
    }

    @Override
    public void onAudio(ByteBuffer buffer) {
        if (!initialized) {
            throw new RuntimeException("Not initialized yet.");
        }
        FloatBuffer floatBuffer = buffer.asFloatBuffer().asReadOnlyBuffer();
        for (int i = 0; i < floatBuffer.capacity(); i += stepSize) {
            double dfOutput = processAudio(floatBuffer, i, stepSize);
            notifyOutputListeners(dfOutput);
            if (calls_count++ % 100 == 0) {
                Log.i(TAG, "onAudio: " + dfOutput);
            }
        }
    }

    private void notifyOutputListeners(double dfOutput) {
        synchronized (this) {
            for (OnProcessorOutputListener listener : outputListeners) {
                listener.onProcessorOutput(dfOutput);
            }
        }
    }

    @Override
    public void addOnProcessorOutputListener(OnProcessorOutputListener listener) {
        synchronized (this) {
            outputListeners.add(listener);
        }
    }

    @Override
    public void removeOnProcessorOutputListener(OnProcessorOutputListener listener) {
        synchronized (this) {
            outputListeners.remove(listener);
        }
    }
}
