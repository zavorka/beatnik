package re.bass.beatnik.audio;

import android.util.Log;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

import re.bass.beatnik.BeatnikOptions;

/**
 * Created by curly on 24/06/2016.
 */

public class DFAudioProcessor implements AudioProcessor
{
    private final String TAG = "DFAudioProcessor";

    private final int stepSize;
    private final int windowSize;
    private final int sampleRate;
    private boolean initialized = false;

    private final float[] fftBuffer;
    private final float[] magnitudes;

    private final List<OnProcessorOutputListener> outputListeners =
            new ArrayList<>();

    public DFAudioProcessor(BeatnikOptions options) {
        sampleRate = options.getSampleRate();
        stepSize = options.getStepSize();
        windowSize = options.getWindowSize();

        fftBuffer = new float[windowSize + 2];
        magnitudes = new float[fftBuffer.length / 2];
    }

    private native void init(
            int sampleRate,
            int stepSize,
            int windowSize
    );
    private native double processAudio(
            float[] buffer,
            int offset,
            int size,
            float[] fftBuffer,
            float[] magnitudes
    );

    @Override
    public void onStart() {
        init(sampleRate, stepSize, windowSize);
        Log.v(TAG, "Initialized.");
        this.initialized = true;
    }

    @Override
    public void onAudio(float[] buffer) {
        if (!initialized) {
            throw new RuntimeException("Not initialized yet.");
        }

        for (int i = 0; i < buffer.length; i += stepSize) {
            double dfOutput = processAudio(
                    buffer,
                    i,
                    stepSize,
                    fftBuffer,
                    magnitudes
            );
            notifyOutputListeners(dfOutput);
        }
    }

    private void notifyOutputListeners(double dfOutput) {
        synchronized (this) {
            for (OnProcessorOutputListener listener : outputListeners) {
                listener.onProcessorOutput(dfOutput, fftBuffer, magnitudes);
            }
        }
    }

    @Override
    public void addOnProcessorOutputListener(
            OnProcessorOutputListener listener
    ) {
        synchronized (this) {
            outputListeners.add(listener);
        }
    }

    @Override
    public void removeOnProcessorOutputListener(
            OnProcessorOutputListener listener
    ) {
        synchronized (this) {
            outputListeners.remove(listener);
        }
    }

    @Override
    public int getWindowSize() {
        return windowSize;
    }

    @Override
    public int getFFTSize() {
        return getWindowSize() / 2 + 1;
    }
}
