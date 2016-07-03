package re.bass.beatnik.audio;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder;
import android.os.Process;
import android.os.SystemClock;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.util.ArrayList;
import java.util.List;

import re.bass.beatnik.BeatnikOptions;

import static android.media.AudioManager.STREAM_MUSIC;

/**
 * Created by curly on 22/06/2016.
 */

public class Microphone extends Thread
        implements AudioInput
{
    private final static String TAG = "Microphone";

    private int sampleRate;
    private int stepSize;

    private final static int AUDIO_SOURCE = MediaRecorder.AudioSource.MIC;
    private final static short ENCODING = AudioFormat.ENCODING_PCM_16BIT;
    private final static int CHANNEL_MASK = AudioFormat.CHANNEL_IN_MONO;

    private final static int NOTIFICATION_PERIOD_IN_BUFFERS = 100;

    private short[] shortBuffer;
    private float[] buffer;
    private int bufferSize = 0;

    private boolean started = false;

    private List<AudioListener> listeners = new ArrayList<>();

    public Microphone(BeatnikOptions options) {
        this.sampleRate = options.getSampleRate();
        this.stepSize = options.getStepSize();

        buffer = new float[getBufferSize()];
        shortBuffer = new short[getBufferSize()];
    }

    @Override
    public void run() {
        Process.setThreadPriority(Process.THREAD_PRIORITY_AUDIO);
        AudioRecord record = buildAudioRecord();

        if (record.getRecordingState() == AudioRecord.RECORDSTATE_RECORDING) {
            throw new RuntimeException(
                    "Recording has already started! (it shouldn't have)"
            );
        }

        SampleRateMonitor monitor = new SampleRateMonitor();
        addListener(monitor);
        record.setPositionNotificationPeriod(
                getBufferSize() * NOTIFICATION_PERIOD_IN_BUFFERS
        );
        Log.v(TAG, String.format(
                "Position notification period set to %d frames",
                NOTIFICATION_PERIOD_IN_BUFFERS * getBufferSize()
        ));
        record.setRecordPositionUpdateListener(monitor);

        record.startRecording();
        notifyOnStartListeners();

        while (record.getRecordingState() == AudioRecord.RECORDSTATE_RECORDING) {
            record.read(shortBuffer, 0, shortBuffer.length);
            for (int i = 0; i < shortBuffer.length; i++) {
                buffer[i] = ((float) shortBuffer[i]) / ((float) Short.MAX_VALUE);
            }
            notifyOnAudioListeners();
        }
    }

    private void notifyOnStartListeners() {
        synchronized (this) {
            started = true;
            for (AudioListener listener : listeners) {
                listener.onStart();
            }
        }
    }

    private void notifyOnAudioListeners() {
        synchronized (this) {
            for (AudioListener listener : listeners) {
                listener.onAudio(buffer);
            }
        }
    }

    @Override
    public void addListener(AudioListener listener) {
        Log.v(TAG, "addListener()");
        synchronized (this) {
            listeners.add(listener);
        }
    }

    @Override
    public void removeListener(AudioListener listener) {
        Log.v(TAG, "removeListener()");
        synchronized (this) {
            listeners.remove(listener);
        }
    }

    @Override
    public int getBufferSize() {
        if (bufferSize <= 0) {
            bufferSize = getPreferredBufferSize();
            Log.i(TAG, String.format(
                    "Preferred buffer size: %d frames.",
                    bufferSize
            ));
        }
        return bufferSize;
    }

    @Override
    public int getBufferSizeInBytes() {
        return getBufferSize() * getFrameSize();
    }

    @Override
    public int getFrameSize() {
        return Float.SIZE / Byte.SIZE;
    }

    @Override
    public void startFetchingAudio() {
        start();
    }

    private int getPreferredBufferSize() {
        final int minSize = AudioTrack.getMinBufferSize(
                sampleRate,
                AudioFormat.CHANNEL_OUT_MONO,
                ENCODING // get number of frames instead of bytes
        ) / getFrameSize();
        int steps = 1;
        while (stepSize * steps < minSize) {
            steps *= 2;
        }
        Log.i(TAG, String.format(
                "Reported minimal buffer size: %d frames",
                minSize
        ));
        Log.i(TAG, String.format(
                "Reported native audio sample rate: %d",
                AudioTrack.getNativeOutputSampleRate(STREAM_MUSIC)
        ));
        return stepSize * steps;
    }

    private AudioRecord buildAudioRecord () {
        return new AudioRecord(
                AUDIO_SOURCE,
                sampleRate,
                CHANNEL_MASK,
                ENCODING,
                getBufferSize() * (Short.SIZE / Byte.SIZE)
        );
    }

    private class SampleRateMonitor
            implements
            AudioRecord.OnRecordPositionUpdateListener,
            AudioListener
    {
        private int receivedSamplesSinceLastNotification = 0;
        private float lastReceivedSamplesRate = .0f;

        private static final float RECEIVED_RATE_EPSILON = 1.f
                / (NOTIFICATION_PERIOD_IN_BUFFERS - 1);
        private static final String TAG = "SampleRateMonitor";

        @Override
        public void onMarkerReached(AudioRecord recorder) {

        }

        @Override
        public void onPeriodicNotification(AudioRecord recorder) {
            float receivedSamplesRate;
            float notificationPeriod = recorder.getPositionNotificationPeriod();

            synchronized (this) {
                receivedSamplesRate = receivedSamplesSinceLastNotification
                        / notificationPeriod;
                receivedSamplesSinceLastNotification = 0;
            }

            if (Math.abs(lastReceivedSamplesRate - receivedSamplesRate)
                    > RECEIVED_RATE_EPSILON) {
                Log.w(TAG, String.format(
                        "Received-samples rate changed from %f to %f",
                        lastReceivedSamplesRate,
                        receivedSamplesRate
                ));
            }

            lastReceivedSamplesRate = receivedSamplesRate;
        }

        @Override
        public void onAudio(float[] buffer) {
            synchronized (this) {
                receivedSamplesSinceLastNotification += getBufferSize();
            }
        }

        @Override
        public void onStart() {
            receivedSamplesSinceLastNotification = 0;
        }
    }
}
