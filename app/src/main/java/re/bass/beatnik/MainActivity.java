package re.bass.beatnik;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import java.nio.ByteBuffer;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        init();

        Microphone microphone = new Microphone();
        microphone.addListener(new AudioReceiver.AudioListener() {
            @Override
            public void onAudio(ByteBuffer buffer) {
                processAudio(buffer);
            }
        });

        microphone.run();
    }

    private native void init();
    private native double processAudio(ByteBuffer buffer);

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("beatnik");
    }
}
