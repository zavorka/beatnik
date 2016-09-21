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

import java.nio.ShortBuffer;

import re.bass.beatnik.BuildConfig;
import re.bass.beatnik.R;
import re.bass.beatnik.audio.AudioInput;
import re.bass.beatnik.audio.Beatnik;
import re.bass.beatnik.audio.Microphone;
import re.bass.beatnik.plot.GLPlotView;
import re.bass.beatnik.plot.PlotUpdater;

// Copyright (c) 2016 Roman Beránek. All rights reserved.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

public class MainActivity extends Activity
{
    private final static String TAG = "MainActivity";

    static {
        System.loadLibrary("beatnik_jni");
    }

    private AudioInput input;
    private PlotUpdater plotUpdater;

    private RelativeLayout content;
    private TextView bpmNumberText;
	private TextView bpmUnitText;
    private TextView fuckOffText;
    private GLPlotView fftView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.v(TAG, "onCreate()");

        setContentView(R.layout.activity_main);

        content = (RelativeLayout) findViewById(R.id.content_layout);
        bpmNumberText = (TextView) findViewById(R.id.bpm_number_text);
        bpmUnitText = (TextView) findViewById(R.id.bpm_unit_text);
        fuckOffText = (TextView) findViewById(R.id.fuck_off_text);
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
        Log.d(TAG, "onStart()");

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            getRecordingPermissionAndStart();
        } else {
            start();
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
        Log.d(TAG, "onStop()");

        input.stop();
        plotUpdater.stop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.d(TAG, "onDestroy()");
    }

    private void initialize() {
        Log.v(TAG, "initialize()");
        bpmNumberText.setText(R.string.app_name);

        Beatnik.clear();
        input = new Microphone(
                Beatnik.getRequiredSampleRate(),
                Beatnik.getRequiredStepSize()
        );

        input.addListener(new AudioInput.AudioListener() {
            @Override
            public void onAudio(final ShortBuffer buffer) {
                if (Beatnik.process(buffer)) {
                    onNewBPM(Beatnik.estimateTempo());
                }
                plotUpdater.requestRender();
            }
        });

        Beatnik.attachFFTPlotBuffer(fftView.getPlotBuffer());
        plotUpdater = new PlotUpdater();
        plotUpdater.addGlSurfaceView(fftView);
    }

    private void startRecording() {
        Log.v(TAG, "startRecording()");
        input.start();
        plotUpdater.start();
    }

    private void doNothing() {
        content.setVisibility(View.GONE);
        fuckOffText.setVisibility(View.VISIBLE);
    }

    protected void onUserChangedMind() {
        getRecordingPermissionAndStart();
    }

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
        Log.d(TAG, "getRecordingPermissionAndStart()");
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
