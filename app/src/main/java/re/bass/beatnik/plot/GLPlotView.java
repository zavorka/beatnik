package re.bass.beatnik.plot;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

import java.nio.FloatBuffer;

import re.bass.beatnik.utils.MultisampleConfigChooser;

/**
 * Created by curly on 7/8/16.
 */

public class GLPlotView extends GLSurfaceView
{
    protected static final int MAX_POINTS = 256;
    protected final PlotRenderer renderer;

    public GLPlotView(Context context) {
        this(context, null);
    }

    public GLPlotView(Context context, AttributeSet attrs) {
        super(context, attrs);

        // Create an OpenGL ES 2.0 context
        setEGLContextClientVersion(2);

        renderer = new PlotRenderer(MAX_POINTS);
        setEGLConfigChooser(new MultisampleConfigChooser());
        setRenderer(renderer);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

    public void updateWithValues(final float[] values) {
        renderer.setPoints(values);
        requestRender();
    }

    public FloatBuffer getPlotBuffer() {
        return renderer.getPlotBuffer();
    }
}
