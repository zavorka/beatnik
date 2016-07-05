package re.bass.beatnik.plot;

import android.content.Context;
import android.util.AttributeSet;

/**
 * Created by curly on 7/3/16.
 */

public class FFTPlotView extends PlotView
{
    private static final String TAG = "FFTPlotView";

    private float[] buffer;

    public FFTPlotView(Context context) {
        super(context);
        initialize();
    }

    public FFTPlotView(
            Context context,
            AttributeSet attrs
    ) {
        super(context, attrs);
        initialize();
    }

    public FFTPlotView(
        Context context,
        AttributeSet attrs,
        int defStyleAttr
    ) {
        super(context, attrs, defStyleAttr);
        initialize();
    }

    @Override
    protected void initialize() {
        super.initialize();
    }

    @Override
    protected void updateBuffer()
    {
        if (buffer == null) {
            return;
        }

        synchronized (this) {
            if (plotBuffer == null || plotBuffer.length != buffer.length) {
                plotBuffer = new float[buffer.length];
            }

            System.arraycopy(buffer, 0, plotBuffer, 0, buffer.length);
        }
    }

    public void updateWithFFTMagnitudes(float[] data) {
        synchronized (this) {
            if (buffer == null || buffer.length != data.length) {
                buffer = new float[data.length];
            }

            System.arraycopy(data, 0, buffer, 0, data.length);
        }
        dataChanged();
    }
}
