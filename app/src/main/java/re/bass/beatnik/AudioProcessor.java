package re.bass.beatnik;

/**
 * Created by curly on 24/06/2016.
 */

interface AudioProcessor {
    void addOnProcessorOutputListener(OnProcessorOutputListener listener);
    void removeOnProcessorOutputListener(OnProcessorOutputListener listener);

    interface OnProcessorOutputListener {
        void onProcessorOutput(double output);
    }
}
