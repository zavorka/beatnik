package re.bass.beatnik;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;


public class MainActivity
        extends AppCompatActivity
        implements BeatAnalyzer.OnBPMCalculatedListener
{
    private final static String TAG = "MainActivity";

    static {
        System.loadLibrary("beatnik");
    }

    private TextView textView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        textView = (TextView)findViewById(R.id.text_view);

        final BeatnikOptions options = new BeatnikOptions();
        Microphone microphone = new Microphone(options);
        DFAudioProcessor processor = new DFAudioProcessor(options);
        BeatAnalyzer analyzer = new BeatAnalyzer(options);

        microphone.addListener(processor);
        processor.addOnProcessorOutputListener(analyzer);
        analyzer.addOnBPMCalculatedListener(this);

        microphone.start();
        analyzer.start();
    }

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
}
