package re.bass.beatnik;

import java.nio.ByteBuffer;
import java.nio.DoubleBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by curly on 24/06/2016.
 */

class BeatAnalyzer implements AudioProcessor.OnProcessorOutputListener
{
    private static final int DEFAULT_BUFFER_SIZE = 2048;

    private final DoubleBuffer dfOutput;

    private boolean ongoingAnalysis = false;

    private List<OnBPMCalculatedListener> listeners = new ArrayList<>();

    BeatAnalyzer(BeatnikOptions options) {
        this(options, DEFAULT_BUFFER_SIZE);
    }

    BeatAnalyzer(BeatnikOptions options, int bufferSize) {
        dfOutput = ByteBuffer.allocateDirect(bufferSize * Double.SIZE).asDoubleBuffer();
        dfOutput.clear();
        init(options.getSampleRate(), options.getStepSize(), options.getWindowSize());
    }

    void start() {
        Timer calculateBPMTimer = new Timer(true);
        calculateBPMTimer.scheduleAtFixedRate(new TimerTask() {
            @Override
            public void run() {
                synchronized (this) {
                    ongoingAnalysis = true;
                }
                float bpm = getBPM();
                notifyBPMCalculated(bpm);
                synchronized (this) {
                    ongoingAnalysis = false;
                }
            }
        }, 10000, 5000);
    }

    private native void init(int sampleRate, int stepSize, int blockSize);
    private native void enqueueDFValue(double dfValue);
    private native float getBPM();

    @Override
    public void onProcessorOutput(double output) {
        synchronized (this) {
            if (ongoingAnalysis) {
                dfOutput.put(output);
            } else {
                dfOutput.flip();
                int limit = dfOutput.limit();
                for (int i = 0; i < limit; i++) {
                    enqueueDFValue(dfOutput.get(i));
                }
                dfOutput.clear();
                enqueueDFValue(output);
            }
        }
    }

    void addOnBPMCalculatedListener(OnBPMCalculatedListener listener) {
        synchronized (this) {
            listeners.add(listener);
        }
    }

    void removeOnBPMCalculatedListener(OnBPMCalculatedListener listener) {
        synchronized (this) {
            listeners.remove(listener);
        }
    }

    private void notifyBPMCalculated(float bpm) {
        synchronized (this) {
            for (OnBPMCalculatedListener listener : listeners) {
                listener.onBPMCalculated(bpm);
            }
        }
    }

    interface OnBPMCalculatedListener {
        void onBPMCalculated(float bpm);
    }
}
