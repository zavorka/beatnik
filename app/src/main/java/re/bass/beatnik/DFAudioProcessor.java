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

    private final List<OnProcessorOutputListener> outputListeners = new ArrayList<>();

    DFAudioProcessor(BeatnikOptions options) {
        sampleRate = options.getSampleRate();
        stepSize = options.getStepSize();
        windowSize = options.getWindowSize();
    }

    private native void init(int sampleRate, int stepSize, int windowSize);
    private native double processAudio(ByteBuffer buffer, int offset, int size);

    @Override
    public void onStart() {
        init(sampleRate, stepSize, windowSize);
        this.initialized = true;
    }

    @Override
    public void onAudio(ByteBuffer buffer) {
        if (!initialized) {
            throw new RuntimeException("Not initialized yet.");
        }
        final int capacityInFloats = buffer.capacity() / (Float.SIZE / Byte.SIZE);
        for (int i = 0; i < capacityInFloats; i += stepSize) {
            double dfOutput = processAudio(buffer, i, stepSize);
            notifyOutputListeners(dfOutput);
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
