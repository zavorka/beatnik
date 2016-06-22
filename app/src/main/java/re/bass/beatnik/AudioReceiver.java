package re.bass.beatnik;

import java.nio.ByteBuffer;

/**
 * Created by curly on 22/06/2016.
 */

interface AudioReceiver extends Runnable {
    void addListener(AudioListener listener); // no pun intended
    void removeListener();

    interface AudioListener {
        void onAudio(ByteBuffer buffer);
    }
}
