package re.bass.beatnik;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder;
import android.os.Process;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by curly on 22/06/2016.
 */

class Microphone extends Thread implements AudioInput
{
    private int sampleRate;
    private int stepSize;

    private final static int AUDIO_SOURCE = MediaRecorder.AudioSource.MIC;
    private final static short ENCODING = AudioFormat.ENCODING_PCM_FLOAT;
    private final static int CHANNEL_MASK = AudioFormat.CHANNEL_IN_MONO;

    private ByteBuffer buffer;
    private int bufferSize = 0;

    private boolean started = false;

    private List<AudioListener> listeners = new ArrayList<>();

    Microphone(BeatnikOptions options) {
        this.sampleRate = options.getSampleRate();
        this.stepSize = options.getStepSize();

        buffer = ByteBuffer.allocateDirect(getBufferSizeInBytes());
        buffer.order(ByteOrder.LITTLE_ENDIAN);
    }

    @Override
    public void run() {
        Process.setThreadPriority(Process.THREAD_PRIORITY_AUDIO);
        AudioRecord record = new AudioRecord.Builder()
                .setAudioSource(AUDIO_SOURCE)
                .setAudioFormat(new AudioFormat.Builder()
                        .setEncoding(ENCODING)
                        .setSampleRate(sampleRate)
                        .setChannelMask(CHANNEL_MASK)
                        .build()
                )
                .setBufferSizeInBytes(getBufferSizeInBytes())
                .build();

        synchronized (this) {
            started = true;
            for (AudioListener listener : listeners) {
                listener.onStart();
            }
        }
        if (record.getRecordingState() != AudioRecord.RECORDSTATE_RECORDING) {
            record.startRecording();
        }


        while (record.getRecordingState() == AudioRecord.RECORDSTATE_RECORDING) {
            record.read(buffer, buffer.capacity(), AudioRecord.READ_BLOCKING);
            synchronized (this) {
                for (AudioListener listener : listeners) {
                    listener.onAudio(buffer);
                }
            }
            // TODO processAudio(audioData);
        }
    }

    @Override
    public void addListener(AudioListener listener) {
        synchronized (this) {
            listeners.add(listener);
        }
    }

    @Override
    public void removeListener(AudioListener listener) {
        synchronized (this) {
            listeners.remove(listener);
        }
    }

    @Override
    public int getBufferSize() {
        if (bufferSize <= 0) {
            bufferSize = getPreferredBufferSize();
        }
        return bufferSize;
    }

    @Override
    public int getBufferSizeInBytes() {
        return getBufferSize() * getFrameSize();
    }

    @Override
    public int getFrameSize() {
        return 4; // Size of float datatype
    }

    private int getPreferredBufferSize() {
        final int minSize = AudioTrack.getMinBufferSize(
                sampleRate,
                AudioFormat.CHANNEL_OUT_MONO,
                AudioFormat.ENCODING_PCM_8BIT // get number of frames instead of bytes
        );
        int steps = 1;
        while (stepSize * steps < minSize) {
            steps *= 2;
        }

        return stepSize * steps;
    }
}
