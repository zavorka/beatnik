package re.bass.beatnik;

import android.Manifest;
import android.content.pm.PackageManager;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.util.TypedValue;
import android.widget.TextView;

import butterknife.BindView;
import butterknife.ButterKnife;

public class MainActivity
        extends AppCompatActivity
        implements BeatAnalyzer.OnBPMCalculatedListener
{
    private final static String TAG = "MainActivity";

    static {
        System.loadLibrary("beatnik");
    }

    private Microphone microphone;
    private BeatAnalyzer analyzer;
	@BindView(R.id.bmp_text) TextView bpmText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        ButterKnife.bind(this);

        initialize();
        getRecordingPermissionAndStart();
    }

    @Override
    protected void onStop() {
        super.onStop();

        if (analyzer != null) {
            analyzer.onStop();
        }
    }

    private void initialize() {
        final BeatnikOptions options = new BeatnikOptions();
        microphone = new Microphone(options);
        DFAudioProcessor processor = new DFAudioProcessor(options);
        analyzer = new BeatAnalyzer(options);

        microphone.addListener(processor);
        processor.addOnProcessorOutputListener(analyzer);
        analyzer.addOnBPMCalculatedListener(this);
    }

    private void startRecording() {
        microphone.start();
        analyzer.start();
    }

    private void doNothing() {
        assert bpmText != null;
        bpmText.setText(R.string.nopeville);
        bpmText.setTextSize(TypedValue.COMPLEX_UNIT_SP, 16);
    }

    @Override
    public void onBPMCalculated(final float bpm) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                assert bpmText != null;
                bpmText.setText(getString(R.string.bpm_value, bpm));
            }
        });
    }

    private void getRecordingPermissionAndStart( /* or die trying */
    ) {
        if (ContextCompat.checkSelfPermission(
                this,
                Manifest.permission.RECORD_AUDIO
        ) != PackageManager.PERMISSION_GRANTED) {
            if (ActivityCompat.shouldShowRequestPermissionRationale(
                    this,
                    Manifest.permission.RECORD_AUDIO
            )) {
                requestAudioRecordingPermission();
            } else {
                requestAudioRecordingPermission();
            }
        } else {
            onPermissionGranted();
        }
    }

    @Override
    public void onRequestPermissionsResult(
            int requestCode,
            @NonNull String permissions[],
            @NonNull int[] grantResults
    ) {
        if (grantResults.length > 0
                && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            onPermissionGranted();
        } else {
            onPermissionDenied();
        }
    }

    private void onPermissionGranted() {
        startRecording();
    }

    private void onPermissionDenied() {
        doNothing();
    }

    private void requestAudioRecordingPermission() {
        ActivityCompat.requestPermissions(
                this,
                new String[] { Manifest.permission.RECORD_AUDIO},
                0);
    }
}
