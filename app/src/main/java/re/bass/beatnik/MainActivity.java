package re.bass.beatnik;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

public class MainActivity extends AppCompatActivity {

    // private final static ... how could you possibly not love Java?
    private final static int SAMPLE_RATE = 44100;
    private final static int STEP_SIZE = 512;
    private final static int WINDOW_SIZE = 1024;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Microphone microphone = new Microphone(SAMPLE_RATE, STEP_SIZE);
        microphone.addListener(new DFAudioProcessor(SAMPLE_RATE, STEP_SIZE, WINDOW_SIZE));

        microphone.start();
    }

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("beatnik");
    }
}
