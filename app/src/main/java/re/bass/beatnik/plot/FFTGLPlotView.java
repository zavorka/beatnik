package re.bass.beatnik.plot;

import android.content.Context;
import android.util.AttributeSet;

/**
 * Created by curly on 7/8/16.
 */

public class FFTGLPlotView extends GLPlotView
{
    private static final String TAG = "FFTGLPlotView";
    private final float[] buffer = new float[MAX_POINTS];

    public FFTGLPlotView(Context context) {
        super(context);
    }

    public FFTGLPlotView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public void updateWithFFTMagnitudes(float[] data) {
        System.arraycopy(data, 0, buffer, 0, Math.min(data.length, buffer.length));
        updateWithValues(buffer);
    }
}
