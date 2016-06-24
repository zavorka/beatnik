package re.bass.beatnik;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;


public class MainActivity extends AppCompatActivity
{
    private final static String TAG = "MainActivity";

    // private final static ... how could you possibly not love Java?
    private final static int SAMPLE_RATE = 44100;
    private final static int STEP_SIZE = 512;
    private final static int WINDOW_SIZE = 1024;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final TextView textView = (TextView)findViewById(R.id.text_view);

        Microphone microphone = new Microphone(SAMPLE_RATE, STEP_SIZE);
        DFAudioProcessor processor = new DFAudioProcessor(SAMPLE_RATE, STEP_SIZE, WINDOW_SIZE);
        BeatAnalyzer analyzer = new BeatAnalyzer(SAMPLE_RATE, STEP_SIZE, WINDOW_SIZE);

        microphone.addListener(processor);
        processor.addOnProcessorOutputListener(analyzer);
        analyzer.addOnBPMCalculatedListener(new BeatAnalyzer.OnBPMCalculatedListener() {
            @Override
            public void onBPMCalculated(final float bpm) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        assert textView != null;
                        textView.setText(getString(R.string.bpm_value, bpm));
                    }
                });
            }
        });

        microphone.start();
        analyzer.start();
    }

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("beatnik");
    }
}
