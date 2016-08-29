package re.bass.beatnik.audio;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder;
import android.os.Process;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ShortBuffer;
import java.util.ArrayList;
import java.util.List;

import static android.media.AudioManager.STREAM_MUSIC;

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

public class Microphone implements AudioInput
{
    class AudioThread extends Thread {
        @Override
        public void run() {
            Process.setThreadPriority(Process.THREAD_PRIORITY_AUDIO);
            AudioRecord record = buildAudioRecord();
            record.startRecording();

            running = true;

            while (running && record.getRecordingState()
                                == AudioRecord.RECORDSTATE_RECORDING) {
                record.read(byteBuffer, byteBuffer.capacity());
                notifyOnAudioListeners();
            }
            record.stop();
            record.release();
        }
    }

    private final static String TAG = "Microphone";
    private static final int MIN_BUFFER_SIZE = 1024;

    private final int sampleRate;
    private final int stepSize;

    private final static int AUDIO_SOURCE = MediaRecorder.AudioSource.MIC;
    private final static short ENCODING = AudioFormat.ENCODING_PCM_16BIT;
    private final static int CHANNEL_MASK = AudioFormat.CHANNEL_IN_MONO;

    private ByteBuffer byteBuffer;
    private ShortBuffer shortBuffer;
    private int bufferSize = 0;

    private boolean running = true;

    private AudioThread thread;

    private final List<AudioListener> listeners = new ArrayList<>();

    public Microphone(int sampleRate, int stepSize) {
        this.sampleRate = sampleRate;
        this.stepSize = stepSize;
        byteBuffer = ByteBuffer.allocateDirect(getBufferSizeInBytes());
        shortBuffer = byteBuffer.asShortBuffer();
    }

    @Override
    public void start() {
        thread = new AudioThread();
        thread.start();
    }

    @Override
    public void stop() {
        running = false;
        try {
            thread.join();
        } catch (InterruptedException e) {
            Thread.interrupted();
        }
        thread = null;
    }

    private void notifyOnAudioListeners() {
        synchronized (listeners) {
            for (int i = 0; i < listeners.size(); ++i) {
                listeners.get(i).onAudio(shortBuffer);
            }
        }
    }

    @Override
    public void addListener(AudioListener listener) {
        Log.v(TAG, "addListener()");
        synchronized (listeners) {
            listeners.add(listener);
        }
    }

    @Override
    public void removeListener(AudioListener listener) {
        Log.v(TAG, "removeListener()");
        synchronized (listeners) {
            listeners.remove(listener);
        }
    }

    @Override
    public int getBufferSize() {
        if (bufferSize <= 0) {
            bufferSize = getPreferredBufferSize();
            Log.i(TAG, String.format(
                    "Preferred buffer size: %d frames.",
                    bufferSize
            ));
        }
        return bufferSize;
    }

    public int getBufferSizeInBytes() {
        return getBufferSize() * getFrameSize();
    }

    @Override
    public int getFrameSize() {
        return Float.SIZE / Byte.SIZE;
    }


    private int getPreferredBufferSize() {
        final int minSize = AudioTrack.getMinBufferSize(
                sampleRate,
                AudioFormat.CHANNEL_OUT_MONO,
                ENCODING // get number of frames instead of bytes
        ) / getFrameSize();
        int steps = 1;
        while (stepSize * steps < minSize) {
            steps *= 2;
        }
        Log.i(TAG, String.format(
                "Reported minimal buffer size: %d frames",
                minSize
        ));
        Log.i(TAG, String.format(
                "Reported native audio sample rate: %d",
                AudioTrack.getNativeOutputSampleRate(STREAM_MUSIC)
        ));
        return Math.max(stepSize * steps, MIN_BUFFER_SIZE);
    }

    private AudioRecord buildAudioRecord () {
        return new AudioRecord(
                AUDIO_SOURCE,
                sampleRate,
                CHANNEL_MASK,
                ENCODING,
                getBufferSize() * (Short.SIZE / Byte.SIZE)
        );
    }
}
