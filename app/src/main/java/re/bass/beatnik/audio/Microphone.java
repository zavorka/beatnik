package re.bass.beatnik.audio;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder;
import android.os.Process;
import android.util.Log;

import java.util.ArrayList;
import java.util.List;

import re.bass.beatnik.BeatnikOptions;

import static android.media.AudioManager.STREAM_MUSIC;

public class Microphone implements AudioInput
{
    class AudioThread extends Thread {
        @Override
        public void run() {
            Process.setThreadPriority(Process.THREAD_PRIORITY_AUDIO);
            AudioRecord record = buildAudioRecord();
            record.startRecording();

            running = true;

            while (running && record.getRecordingState() == AudioRecord.RECORDSTATE_RECORDING) {
                record.read(shortBuffer, 0, shortBuffer.length);
                notifyOnAudioListeners();
            }
            record.stop();
            record.release();
        }
    }

    private final static String TAG = "Microphone";
    private static final int MIN_BUFFER_SIZE = 1024;

    private final int sampleRate;
    private final int stepSize;

    private final static int AUDIO_SOURCE = MediaRecorder.AudioSource.MIC;
    private final static short ENCODING = AudioFormat.ENCODING_PCM_16BIT;
    private final static int CHANNEL_MASK = AudioFormat.CHANNEL_IN_MONO;

    private short[] shortBuffer;
    private int bufferSize = 0;

    private boolean running = true;

    private AudioThread thread;

    private final List<AudioListener> listeners = new ArrayList<>();

    public Microphone(BeatnikOptions options) {
        this.sampleRate = options.getSampleRate();
        this.stepSize = options.getStepSize();
        shortBuffer = new short[getBufferSize()];
    }

    @Override
    public void start() {
        thread = new AudioThread();
        thread.start();
    }

    @Override
    public void stop() {
        running = false;
        try {
            thread.join();
        } catch (InterruptedException e) {
            Thread.interrupted();
        }
        thread = null;
    }

    private void notifyOnAudioListeners() {
        synchronized (listeners) {
            for (AudioListener listener : listeners) {
                listener.onAudio(shortBuffer);
            }
        }
    }

    @Override
    public void addListener(AudioListener listener) {
        Log.v(TAG, "addListener()");
        synchronized (listeners) {
            listeners.add(listener);
        }
    }

    @Override
    public void removeListener(AudioListener listener) {
        Log.v(TAG, "removeListener()");
        synchronized (listeners) {
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
    public int getFrameSize() {
        return Float.SIZE / Byte.SIZE;
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
        return Math.max(stepSize * steps, MIN_BUFFER_SIZE);
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
}
