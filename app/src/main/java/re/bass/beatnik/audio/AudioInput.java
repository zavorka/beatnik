package re.bass.beatnik.audio;

import java.nio.ShortBuffer;

import re.bass.beatnik.Startable;

public interface AudioInput extends Startable {
    void addListener(AudioListener listener); // no pun intended
    void removeListener(AudioListener listener);

    int getBufferSize();

    int getFrameSize();

    interface AudioListener {
        void onAudio(final ShortBuffer buffer);
    }
}
