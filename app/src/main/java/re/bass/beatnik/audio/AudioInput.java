package re.bass.beatnik.audio;

import re.bass.beatnik.Startable;

public interface AudioInput extends Startable {
    void addListener(AudioListener listener); // no pun intended
    void removeListener(AudioListener listener);

    int getBufferSize();

    int getFrameSize();

    interface AudioListener {
        void onAudio(short[] buffer);
    }
}
