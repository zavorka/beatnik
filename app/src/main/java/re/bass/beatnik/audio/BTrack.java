package re.bass.beatnik.audio;

import android.util.Log;

import java.util.ArrayList;
import java.util.List;

import re.bass.beatnik.BeatnikOptions;

/**
 * Created by curly on 7/15/16.
 */

public class BTrack implements DFProcessor.OnProcessorOutputListener
{
    private static final String TAG = "BTrack";
    private final List<OnNewBPMListener> listeners = new ArrayList<>();

    public BTrack(BeatnikOptions options) {
        init(options.getSampleRate(), options.getStepSize());
    }

    private native void init(
            int sampleRate,
            int stepSize
    );

    private native float processDFSample(float sample);
    private native float processDFSamples(float[] sample);

    @Override
    public void onProcessorOutput(DFProcessor sender, float[] output) {
        float bpm = processDFSamples(output);
        if (!Float.isNaN(bpm)) {
            notifyOnNewBPMListeners(bpm);
        }
    }


    public interface OnNewBPMListener {
        void onNewBPM(float bpm);
    }

    public void addOnNewBPMListener(OnNewBPMListener listener) {
        synchronized (listeners) {
            listeners.add(listener);
        }
    }

    public void removeOnNewBPMListener(OnNewBPMListener listener) {
        synchronized (listeners) {
            listeners.remove(listener);
        }
    }

    private void notifyOnNewBPMListeners(float bpm) {
        synchronized (listeners) {
            for (OnNewBPMListener listener : listeners) {
                listener.onNewBPM(bpm);
            }
        }
    }
}
