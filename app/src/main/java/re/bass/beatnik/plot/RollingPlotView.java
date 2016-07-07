package re.bass.beatnik.plot;

import android.content.Context;
import android.util.AttributeSet;

import re.bass.beatnik.utils.CircularFloatBuffer;

/**
 * Created by curly on 7/2/16.
 */

public class RollingPlotView extends PlotView
{
    private static final String TAG = "RollingPlotView";
    private static final int BUFFER_SIZE = 2048;

    private static final int SKIP = 8;
    private int toSkip = 0;

    private CircularFloatBuffer buffer;

    public RollingPlotView(Context context) {
        super(context);
        initialize();
    }

    public RollingPlotView(
            Context context,
            AttributeSet attrs
    ) {
        super(context, attrs);
        initialize();
    }

    public RollingPlotView(
            Context context,
            AttributeSet attrs,
            int defStyleAttr
    ) {
        super(context, attrs, defStyleAttr);
        initialize();
    }

    private boolean initialized = false;
    protected void initialize() {
        super.initialize();
        if (initialized) {
            return;
        }

        buffer = new CircularFloatBuffer(getPlotBufferSize());

        initialized = true;
    }

    @Override
    protected void updateBuffer(float[] out) {
        synchronized (this) {
            buffer.copyFrontTo(out);
        }
    }

    public void appendValue(float value) {
        if ((toSkip = (toSkip - 1) % SKIP) != 0) {
            return;
        }
        synchronized (this) {
            buffer.append(value);
        }
        dataChanged();
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
