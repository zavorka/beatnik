package re.bass.beatnik;

/**
 * Created by curly on 24/06/2016.
 */

class BeatnikOptions {
    private static final int SAMPLE_RATE = 44100;
    private static final int STEP_SIZE = 512;
    private static final int WINDOW_SIZE = 1024;

    private int sampleRate;
    private int stepSize;
    private int windowSize;

    BeatnikOptions() {
        this.sampleRate = SAMPLE_RATE;
        this.stepSize = STEP_SIZE;
        this.windowSize = WINDOW_SIZE;
    }

    BeatnikOptions(int sampleRate, int stepSize, int windowSize) {
        this.sampleRate = sampleRate;
        this.stepSize = stepSize;
        this.windowSize = windowSize;
    }

    int getSampleRate() {
        return sampleRate;
    }

    int getStepSize() {
        return stepSize;
    }

    int getWindowSize() {
        return windowSize;
    }
}
