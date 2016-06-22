package re.bass.beatnik;

import android.annotation.TargetApi;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder;
import android.os.Build;
import android.os.Process;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import java.nio.ByteBuffer;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        short audioFormat = AudioFormat.ENCODING_PCM_FLOAT;
        final int sampleRate = 44100;

        int bufferSize = AudioTrack.getMinBufferSize(
                sampleRate,
                AudioFormat.CHANNEL_OUT_MONO,
                audioFormat
        );

        int steps = 1;
        final int stepSize = 512;

        while (steps * stepSize < bufferSize) {
            steps *= 2;
        }
        bufferSize = stepSize * steps;

        final ByteBuffer audioData = ByteBuffer.allocateDirect(audioFormat);

        //final float[] audioData = new float[bufferSize];

        final AudioRecord record = new AudioRecord(
                MediaRecorder.AudioSource.MIC,
                sampleRate,
                AudioFormat.CHANNEL_IN_MONO,
                audioFormat,
                bufferSize
        );

        int state = record.getState();


        Thread recThread = new Thread() {
            @TargetApi(Build.VERSION_CODES.M)
            @Override
            public void run() {
                Process.setThreadPriority(Process.THREAD_PRIORITY_AUDIO);
                if (record.getRecordingState() != AudioRecord.RECORDSTATE_RECORDING) {
                    record.startRecording();
                }

                while (record.getRecordingState() == AudioRecord.RECORDSTATE_RECORDING) {
                    record.read(audioData, 0, AudioRecord.READ_BLOCKING);
                    processAudio(audioData);
                }
            }
        };

        recThread.run();
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
    private native double processAudio(ByteBuffer buffer);

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("beatnik");
    }
}
