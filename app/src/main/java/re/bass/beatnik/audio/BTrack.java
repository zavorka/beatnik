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

    private native double processDFSample(double sample);

    @Override
    public void onProcessorOutput(double[] output) {
        double newBPM = 0.;
        for (double value : output) {
            double bpm = processDFSample(value);
            // Log.v(TAG, "bpm: " + bpm);
            if (bpm < 0.0) {
                newBPM = bpm;
            }
        }

        if (newBPM < 0.0) {
            notifyOnNewBPMListeners(-newBPM);
        }
    }


    public interface OnNewBPMListener {
        void onNewBPM(double bpm);
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

    private void notifyOnNewBPMListeners(double bpm) {
        synchronized (listeners) {
            for (OnNewBPMListener listener : listeners) {
                listener.onNewBPM(bpm);
            }
        }
    }
}
