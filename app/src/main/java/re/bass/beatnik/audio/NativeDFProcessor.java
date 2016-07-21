package re.bass.beatnik.audio;

import android.util.Log;

import java.nio.FloatBuffer;
import java.util.ArrayList;
import java.util.List;

import re.bass.beatnik.BeatnikOptions;
import re.bass.beatnik.Destroyable;

/**
 * Created by curly on 24/06/2016.
 */

{
    private final String TAG = "NativeDFProcessor";

    private final int stepSize;

    private float[] dfOutput;

    private final List<OnProcessorOutputListener> outputListeners =
            new ArrayList<>();

    public NativeDFProcessor(BeatnikOptions options) {
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
    private native void dealloc();
    private native void processAudio(
            short[] buffer,
            float[] output
    );

    public native void setDFPlotBuffer(FloatBuffer buffer, int length);
    public native void setFFTPlotBuffer(FloatBuffer buffer, int length);


    @Override
    public void onAudio(short[] buffer) {
        if (dfOutput == null || dfOutput.length != (buffer.length / stepSize)) {
            dfOutput = new float[buffer.length / stepSize];
        }

        processAudio(
                buffer,
                dfOutput
        );

        notifyOutputListeners();
    }

    private void notifyOutputListeners() {
        synchronized (outputListeners) {
            for (OnProcessorOutputListener listener : outputListeners) {
                listener.onProcessorOutput(this, dfOutput);
            }
        }
    }

    @Override
    public void addOnDFProcessorOutputListener(
            OnProcessorOutputListener listener
    ) {
        synchronized (outputListeners) {
            outputListeners.add(listener);
        }
    }

    @Override
    public void removeOnDFProcessorOutputListener(
            OnProcessorOutputListener listener
    ) {
        synchronized (outputListeners) {
            outputListeners.remove(listener);
        }
    }

    @Override
    public void destroy() {
        dealloc();
    }
}
