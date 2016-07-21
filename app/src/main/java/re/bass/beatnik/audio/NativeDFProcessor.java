package re.bass.beatnik.audio;

import android.util.Log;

import java.nio.FloatBuffer;
import java.util.ArrayList;
import java.util.List;

import re.bass.beatnik.BeatnikOptions;
import re.bass.beatnik.Destroyable;
import re.bass.beatnik.plot.GLPlotView;

/**
 * Created by curly on 24/06/2016.
 */

public class NativeDFProcessor implements DFProcessor, Destroyable
{
    private final String TAG = "NativeDFProcessor";

    private final int stepSize;

    private float[] dfOutput;

    private final List<OnProcessorOutputListener> outputListeners =
            new ArrayList<>();

    public NativeDFProcessor(BeatnikOptions options) {
        stepSize = options.getStepSize();
        init();
    }

    private native void init();
    private native void dealloc();
    private native void processAudio(
            short[] buffer,
            float[] output
    );

    private native void attachDFPlotBuffer(FloatBuffer buffer, int length);
    private native void attachFFTPlotBuffer(FloatBuffer buffer, int length);
    private native void detachDFPlotBuffer();
    private native void detachFFTPlotBuffer();

    public void attachDFPlotView(GLPlotView plotView) {
        FloatBuffer buffer = plotView.getPlotBuffer();
        attachDFPlotBuffer(buffer, buffer.capacity());
    }
    public void attachFFTPlotView(GLPlotView plotView) {
        FloatBuffer buffer = plotView.getPlotBuffer();
        attachFFTPlotBuffer(buffer, buffer.capacity());
    }

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
