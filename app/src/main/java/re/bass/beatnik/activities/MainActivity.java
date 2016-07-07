package re.bass.beatnik.activities;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.RelativeLayout;
import android.widget.TextView;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;
import re.bass.beatnik.BeatnikOptions;
import re.bass.beatnik.R;
import re.bass.beatnik.audio.AudioInput;
import re.bass.beatnik.audio.FFTProcessor;
import re.bass.beatnik.audio.NativeDFProcessor;
import re.bass.beatnik.audio.DFProcessor;
import re.bass.beatnik.audio.BeatAnalyzer;
import re.bass.beatnik.audio.Microphone;
import re.bass.beatnik.plot.FFTPlotView;
import re.bass.beatnik.plot.RollingPlotView;

public class MainActivity
        extends AppCompatActivity
        implements BeatAnalyzer.OnBPMCalculatedListener
{
    private final static String TAG = "MainActivity";

    static {
        System.loadLibrary("beatnik");
    }

    private AudioInput input;
    private BeatAnalyzer analyzer;
    private NativeDFProcessor processor;

    @BindView(R.id.content_layout) RelativeLayout content;
    @BindView(R.id.bpm_number_text) TextView bpmNumberText;
	@BindView(R.id.bpm_unit_text) TextView bpmUnitText;
    @BindView(R.id.fuck_off_text) TextView fuckOffText;
    @BindView(R.id.df_view) RollingPlotView dfView;
    @BindView(R.id.fft_view) FFTPlotView fftView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.v(TAG, "onCreate()");

        setContentView(R.layout.activity_main);
        ButterKnife.bind(this);

        initialize();
        getRecordingPermissionAndStart();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        input.stopFetchingAudio();
        analyzer.stop();
        processor.destroy();
        dfView.stop();
        fftView.stop();
    }

    @Override
    protected void onStop() {
        super.onStop();
        Log.v(TAG, "stop()");
    }

    private void initialize() {
        Log.v(TAG, "initialize()");
        bpmNumberText.setText(R.string.app_name);

        final BeatnikOptions options = new BeatnikOptions();
        input = new Microphone(options);
        processor = new NativeDFProcessor(options);
        analyzer = new BeatAnalyzer(options);

        input.addListener(processor);
        processor.addOnNewFFTDataListener(
                new FFTProcessor.OnNewFFTDataListener() {
                    @Override
                    public void onNewFFTData(
                            FFTProcessor sender
                    ) {
                        fftView.updateWithFFTMagnitudes(sender.getMagnitudes());
                    }
                }
        );
        processor.addOnDFProcessorOutputListener(
                new DFProcessor.OnProcessorOutputListener() {
                    @Override
                    public void onProcessorOutput(
                            double[] output
                    ) {
                        // a)
                        /*
                        double mean = 0;
                        for (double value : output) {
                            mean += value / output.length;
                        }
                        dfView.appendValue((float) mean);
                        */

                        // b)
                        // dfView.appendArray(output);

                        // c)
                        dfView.appendValue((float) output[0]);
                    }
        });

        processor.addOnDFProcessorOutputListener(analyzer);
        analyzer.addOnBPMCalculatedListener(this);
    }

    private void startRecording() {
        Log.v(TAG, "startRecording()");
        input.startFetchingAudio();
        analyzer.start();
    }

    private void doNothing() {
        content.setVisibility(View.GONE);
        fuckOffText.setVisibility(View.VISIBLE);
    }

    @OnClick(R.id.fuck_off_text)
    protected void onUserChangedMind() {
        getRecordingPermissionAndStart();
    }

    @Override
    public void onBPMCalculated(final float bpm) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Log.v(TAG, "BPM calculated: " + String.valueOf(bpm));
                if (Float.isNaN(bpm)) {
                    return;
                }
                bpmUnitText.setVisibility(View.VISIBLE);
                bpmNumberText.setText(getString(R.string.bpm_value, bpm));
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
                Log.v(TAG, "SHOULD show request permission rationale");
                requestAudioRecordingPermission();
            } else {
                Log.v(TAG, "should NOT show request permission rationale");
                requestAudioRecordingPermission();
            }
        } else {
            Log.v(TAG, "no need to request recording permission");
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
        Log.d(TAG, "Mic permission granted");
        fuckOffText.setVisibility(View.GONE);
        content.setVisibility(View.VISIBLE);
        startRecording();
    }

    private void onPermissionDenied() {
        Log.d(TAG, "Mic permission denied.");
        doNothing();
    }

    private void requestAudioRecordingPermission() {
        Log.d(TAG, "Requesting mic permission.");
        ActivityCompat.requestPermissions(
                this,
                new String[] { Manifest.permission.RECORD_AUDIO},
                0);
    }
}
