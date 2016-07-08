package re.bass.beatnik.audio;

import re.bass.beatnik.Startable;

/**
 * Created by curly on 22/06/2016.
 */

public interface AudioInput extends Startable {
    void addListener(AudioListener listener); // no pun intended
    void removeListener(AudioListener listener);

    int getBufferSize();
    int getBufferSizeInBytes();
    int getFrameSize();

    interface AudioListener {
        void onAudio(float[] buffer);
        void onStart();
    }
}
