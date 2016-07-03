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
    private static final int BUFFER_SIZE = 512;

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

        buffer = new CircularFloatBuffer(BUFFER_SIZE);
        plotBuffer = new float[BUFFER_SIZE];

        initialized = true;
    }

    @Override
    protected void updateBuffer() {
        try {
            synchronized (this) {
                buffer.copyFrontTo(plotBuffer);
            }
        } catch (CircularFloatBuffer.ArrayTooLargeException e) {
            e.printStackTrace();
        }
    }

    public void appendValue(float value) {
        synchronized (this) {
            buffer.append(value);
        }
        dataChanged();
    }

    public void appendArray(float[] array) {
        synchronized (this) {
            buffer.appendArray(array);
        }
        dataChanged();
    }
}
