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

public class NativeDFProcessor implements DFProcessor, FFTProcessor, Destroyable
{
    private final String TAG = "NativeDFProcessor";

    private final int stepSize;
    private final int windowSize;
    private final int sampleRate;
    private boolean initialized = false;

    private final float[] fftBuffer;
    private boolean fftBufferUpToDate = false;
    private final float[] magnitudes;
    private boolean magnitudesUpToDate = false;

    private double[] dfOutput;

    private final List<OnProcessorOutputListener> outputListeners =
            new ArrayList<>();
    private final List<OnNewFFTDataListener> fftListeners =
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
            double[] output
    );
    private native void getFrequencyData(
            float[] buffer
    );
    private native void calculateMagnitudes(
            float[] buffer
    );

    public native void setDFPlotBuffer(FloatBuffer buffer, int length);
    public native void setFFTPlotBuffer(FloatBuffer buffer, int length);

    @Override
    public void onStart() {
        init(sampleRate, stepSize, windowSize);
        Log.v(TAG, "Initialized.");
        this.initialized = true;
    }

    @Override
    public void onAudio(short[] buffer) {
        if (!initialized) {
            throw new RuntimeException("Not initialized yet.");
        }

        if (dfOutput == null || dfOutput.length != (buffer.length / stepSize)) {
            dfOutput = new double[buffer.length / stepSize];
        }

        processAudio(
                buffer,
                dfOutput
        );

        fftBufferUpToDate = false;
        magnitudesUpToDate = false;

        notifyFFTDataListeners();
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

    private void notifyFFTDataListeners() {
        synchronized (fftListeners) {
            for (OnNewFFTDataListener listener : fftListeners) {
                listener.onNewFFTData(this);
            }
        }
    }

    @Override
    public void addOnNewFFTDataListener(
            OnNewFFTDataListener listener
    ) {
        synchronized (fftListeners) {
            fftListeners.add(listener);
        }
    }

    @Override
    public void removeOnNewFFTDataListener(
            OnNewFFTDataListener listener
    ) {
        synchronized (fftListeners) {
            fftListeners.remove(listener);
        }
    }

    @Override
    public float[] getFFTBuffer() {
        if (!fftBufferUpToDate) {
            getFrequencyData(fftBuffer);
            fftBufferUpToDate = true;
        }
        return fftBuffer;
    }

    @Override
    public float[] getMagnitudes() {
        if (!magnitudesUpToDate) {
            calculateMagnitudes(magnitudes);
            magnitudesUpToDate = true;
        }
        return magnitudes;
    }

    @Override
    public int getWindowSize() {
        return windowSize;
    }

    @Override
    public int getFFTSize() {
        return getWindowSize() / 2 + 1;
    }

    @Override
    public void destroy() {
        dealloc();
    }
}
