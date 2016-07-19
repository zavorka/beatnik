package re.bass.beatnik.activities;

import android.Manifest;
import android.annotation.TargetApi;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.util.Log;
import android.view.View;
import android.widget.RelativeLayout;
import android.widget.TextView;

import re.bass.beatnik.BeatnikOptions;
import re.bass.beatnik.BuildConfig;
import re.bass.beatnik.R;
import re.bass.beatnik.audio.AudioInput;
import re.bass.beatnik.audio.BTrack;
import re.bass.beatnik.audio.DFProcessor;
import re.bass.beatnik.audio.FFTProcessor;
import re.bass.beatnik.audio.Microphone;
import re.bass.beatnik.audio.NativeDFProcessor;
import re.bass.beatnik.plot.GLPlotView;

public class MainActivity
        extends Activity
        implements BTrack.OnNewBPMListener
{
    private final static String TAG = "MainActivity";

    static {
        System.loadLibrary("beatnik");
    }

    private AudioInput input;
    private NativeDFProcessor processor;
    private BTrack bTrack;

    RelativeLayout content;
    TextView bpmNumberText;
	TextView bpmUnitText;
    TextView fuckOffText;
    GLPlotView dfView;
    GLPlotView fftView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.v(TAG, "onCreate()");

        setContentView(R.layout.activity_main);

        content = (RelativeLayout) findViewById(R.id.content_layout);
        bpmNumberText = (TextView) findViewById(R.id.bpm_number_text);
        bpmUnitText = (TextView) findViewById(R.id.bpm_unit_text);
        fuckOffText = (TextView) findViewById(R.id.fuck_off_text);
        dfView = (GLPlotView) findViewById(R.id.df_view);
        fftView = (GLPlotView) findViewById(R.id.fft_view);
        fuckOffText.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                onUserChangedMind();
            }
        });

        initialize();
    }

    @Override
    protected void onStart() {
        super.onStart();

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            getRecordingPermissionAndStart();
        } else {
            start();
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
        Log.v(TAG, "stop()");

        input.stop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        processor.destroy();
    }

    private void initialize() {
        Log.v(TAG, "initialize()");
        bpmNumberText.setText(R.string.app_name);

        final BeatnikOptions options = new BeatnikOptions();
        input = new Microphone(options);
        processor = new NativeDFProcessor(options);
        bTrack = new BTrack(options);

        input.addListener(processor);

        processor.setFFTPlotBuffer(fftView.getPlotBuffer(), fftView.getPlotBuffer().capacity());
        processor.addOnNewFFTDataListener(new FFTProcessor.OnNewFFTDataListener() {
            @Override
            public void onNewFFTData(FFTProcessor sender) {
                fftView.requestRender();
            }
        });
        processor.setDFPlotBuffer(dfView.getPlotBuffer(), dfView.getPlotBuffer().capacity());
        processor.addOnDFProcessorOutputListener(new DFProcessor.OnProcessorOutputListener() {
            @Override
            public void onProcessorOutput(DFProcessor sender, float[] output) {
                dfView.requestRender();
            }
        });

        processor.addOnDFProcessorOutputListener(bTrack);
        bTrack.addOnNewBPMListener(this);
    }

    private void startRecording() {
        Log.v(TAG, "startRecording()");
        input.start();
    }

    private void doNothing() {
        content.setVisibility(View.GONE);
        fuckOffText.setVisibility(View.VISIBLE);
    }

    protected void onUserChangedMind() {
        getRecordingPermissionAndStart();
    }

    @Override
    public void onNewBPM(final float bpm) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (BuildConfig.DEBUG) {
                    Log.v(TAG, "BPM calculated: " + String.valueOf(bpm));
                }

                bpmUnitText.setVisibility(View.VISIBLE);
                bpmNumberText.setText(getString(R.string.bpm_value, bpm));
            }
        });
    }

    private void start() {
        onPermissionGranted();
    }

    @TargetApi(Build.VERSION_CODES.M)
    private void getRecordingPermissionAndStart( /* or die trying */
    ) {
        if (checkSelfPermission(
                Manifest.permission.RECORD_AUDIO
        ) != PackageManager.PERMISSION_GRANTED) {
            if (shouldShowRequestPermissionRationale(
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

    @TargetApi(Build.VERSION_CODES.M)
    private void requestAudioRecordingPermission() {
        Log.d(TAG, "Requesting mic permission.");
        requestPermissions(
                new String[] { Manifest.permission.RECORD_AUDIO},
                0);
    }
}
