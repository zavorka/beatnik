package re.bass.beatnik.audio;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by curly on 7/15/16.
 */

public class BTrack implements DFProcessor.OnProcessorOutputListener
{
    private static final String TAG = "BTrack";
    private final List<OnNewBPMListener> listeners = new ArrayList<>();

    public BTrack() {
        init();
    }

    private native void init();

    private native boolean processDFSample(float sample);
    private native boolean processDFSamples(float[] sample);

    private native float getBpm();

    @Override
    public void onProcessorOutput(DFProcessor sender, float[] output) {
        if(processDFSamples(output)) {
           notifyOnNewBPMListeners(getBpm());
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
