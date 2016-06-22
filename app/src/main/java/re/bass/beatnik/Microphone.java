package re.bass.beatnik;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder;
import android.os.Process;

import java.nio.ByteBuffer;

/**
 * Created by curly on 22/06/2016.
 */

class Microphone extends Thread implements AudioReceiver
{
    // private final static ... how could you possibly not love Java?
    private final static int SAMPLE_RATE = 44100;
    private final static int STEP_SIZE = 512;
    private final static int WINDOW_SIZE = 1024;
    private final static short NUM_CHANNELS = 1;

    private final static int AUDIO_SOURCE = MediaRecorder.AudioSource.MIC;
    private final static short ENCODING = AudioFormat.ENCODING_PCM_FLOAT;
    private final static int CHANNEL_MASK = AudioFormat.CHANNEL_IN_MONO;

    private AudioRecord record;
    private ByteBuffer buffer;
    private int bufferSize = 0;


    Microphone() {
        buffer = ByteBuffer.allocateDirect(getBufferSizeInBytes());

        record = new AudioRecord.Builder()
                .setAudioSource(AUDIO_SOURCE)
                .setAudioFormat(new AudioFormat.Builder()
                        .setEncoding(ENCODING)
                        .setSampleRate(SAMPLE_RATE)
                        .setChannelMask(CHANNEL_MASK)
                        .build()
                )
                .setBufferSizeInBytes(getBufferSizeInBytes())
                .build();
    }

    @Override
    public void run() {
        Process.setThreadPriority(Process.THREAD_PRIORITY_AUDIO);
        if (record.getRecordingState() != AudioRecord.RECORDSTATE_RECORDING) {
            record.startRecording();
        }

        while (record.getRecordingState() == AudioRecord.RECORDSTATE_RECORDING) {
            record.read(buffer, 0, AudioRecord.READ_BLOCKING);
            // TODO notify listeners
            // TODO processAudio(audioData);
        }
    }

    @Override
    public void addListener(AudioListener listener) {

    }

    @Override
    public void removeListener() {

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
                SAMPLE_RATE,
                AudioFormat.CHANNEL_OUT_MONO,
                ENCODING
        );
        int steps = 1;
        while (STEP_SIZE * steps < minSize) {
            steps *= 2;
        }

        return STEP_SIZE * steps;
    }
}
