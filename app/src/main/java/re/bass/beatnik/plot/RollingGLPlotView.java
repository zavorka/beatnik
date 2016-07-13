package re.bass.beatnik.plot;

import android.content.Context;
import android.util.AttributeSet;

import re.bass.beatnik.utils.CircularFloatBuffer;

/**
 * Created by curly on 7/9/16.
 */

public class RollingGLPlotView extends GLPlotView
{
    private static final String TAG = "RollingGLPlotView";
    private static final int SKIP = 4;

    private CircularFloatBuffer buffer;
    private float[] copy;
    private int toSkip = 0;

    public RollingGLPlotView(Context context) {
        this(context, null);
    }

    public RollingGLPlotView(Context context, AttributeSet attrs) {
        super(context, attrs);

        buffer = new CircularFloatBuffer(MAX_POINTS);
        copy = new float[MAX_POINTS];
    }

    private void update() {
        synchronized (this) {
            buffer.copyFrontTo(copy);
        }
        updateWithValues(copy);
    }

    public void appendValue(float value) {
        if ((toSkip = (toSkip - 1) % SKIP) != 0) {
            return;
        }
        synchronized (this) {
            buffer.append(value);
        }
        update();
    }

    public void appendArray(float[] array) {
        for (float value : array) {
            appendValue(value);
        }
    }

    public void appendArray(double[] array) {
        for (double value : array) {
            appendValue((float) value);
        }
    }
}
