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
    protected void updateBuffer(float[] out)
    {
        if (buffer == null) {
            return;
        }
        int length = (buffer.length - 1) / 2;

        synchronized (this) {
            if (out.length == length) {
                System.arraycopy(buffer, 0, out, 0, out.length);
            }
            else if (out.length > length) {
                int ratio = out.length / length;
                for (int i = 0; i < buffer.length - 1; i++) {
                    for (int j = 0; j < ratio; j++) {
                        out[i * ratio + j] = ((ratio - j) * buffer[i] + j * buffer[i + 1]) / ratio;
                    }
                }
            } else {
                int ratio = length / out.length;
                for (int i = 0; i < out.length; i++) {
                    out[i] = 0;
                    for (int j = 0; j < ratio; j++) {
                        out[i] += buffer[i * ratio + j] / ratio;
                    }
                }
            }
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
