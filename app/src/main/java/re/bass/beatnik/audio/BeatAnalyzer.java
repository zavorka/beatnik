package re.bass.beatnik.audio;

import java.nio.ByteBuffer;
import java.nio.DoubleBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.CountDownLatch;

import re.bass.beatnik.BeatnikOptions;
import re.bass.beatnik.OnStopListener;

/**
 * Created by curly on 24/06/2016.
 */

public class BeatAnalyzer
        implements AudioProcessor.OnProcessorOutputListener,
        OnStopListener
{
    private static final int DEFAULT_BUFFER_SIZE = 2048;

    private final DoubleBuffer tempBuffer;

    private volatile boolean ongoingAnalysis = false;
    private volatile CountDownLatch latch;

    private List<OnBPMCalculatedListener> listeners = new ArrayList<>();

    public BeatAnalyzer(BeatnikOptions options) {
        this(options, DEFAULT_BUFFER_SIZE);
    }

    public BeatAnalyzer(BeatnikOptions options, int bufferSize) {
        tempBuffer = ByteBuffer.allocateDirect(bufferSize * Double.SIZE)
                .asDoubleBuffer();
        tempBuffer.clear();
        init(
                options.getSampleRate(),
                options.getStepSize(),
                options.getWindowSize()
        );
    }

    private synchronized boolean isOngoingAnalysis() {
        return ongoingAnalysis;
    }


    private native void init(
            int sampleRate,
            int stepSize,
            int blockSize
    );
    private native void enqueueDFValue(double dfValue);
    private native float getBPM();
    private native void clearData();


    @Override
    public void onProcessorOutput(double output) {
        synchronized (this) {
            if (ongoingAnalysis) {
                tempBuffer.put(output);
            } else {
                tempBuffer.flip();
                int limit = tempBuffer.limit();
                for (int i = 0; i < limit; i++) {
                    enqueueDFValue(tempBuffer.get(i));
                }
                tempBuffer.clear();
                enqueueDFValue(output);
            }
        }
    }

    public void start() {
        Timer calculateBPMTimer = new Timer(true);
        calculateBPMTimer.scheduleAtFixedRate(new TimerTask() {
            @Override
            public void run() {
                latch = new CountDownLatch(1);
                synchronized (this) {
                    ongoingAnalysis = true;
                }
                float bpm = getBPM();
                notifyBPMCalculated(bpm);
                synchronized (this) {
                    ongoingAnalysis = false;
                    latch.countDown();
                    latch = null;
                }
            }
        }, 10000, 5000); // TODO fixme!
    }

    @Override
    public void onStop() {
        synchronized (this) {
            if (latch == null) {
                clear();
                return;
            }
        }
        try {
            latch.await();
        } catch (InterruptedException e) {
            clear();
            return;
        }
        synchronized (this) {
            clear();
        }
    }

    private void clear() {
        tempBuffer.clear();
        clearData();
    }

    public interface OnBPMCalculatedListener {
        void onBPMCalculated(float bpm);
    }

    public void addOnBPMCalculatedListener(OnBPMCalculatedListener listener) {
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
}
