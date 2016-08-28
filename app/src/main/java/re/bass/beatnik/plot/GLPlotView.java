package re.bass.beatnik.plot;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.util.AttributeSet;
import android.util.Log;

import java.nio.FloatBuffer;

import re.bass.beatnik.utils.MultisampleConfigChooser;

/**
 * Created by curly on 7/8/16.
 */

public class GLPlotView extends GLSurfaceView
{
    private static final String TAG = "GLPlotView";

    protected static final int POINTS_COUNT = 256;
    protected final PlotRenderer renderer;

    public GLPlotView(Context context) {
        this(context, null);
    }

    public GLPlotView(Context context, AttributeSet attrs) {
        super(context, attrs);

        // Create an OpenGL ES 2.0 context
        setEGLContextClientVersion(2);

        renderer = new PlotRenderer(POINTS_COUNT);

        Log.v(TAG, Build.MODEL);
        if (android.os.Build.MODEL.equals("Android SDK built for x86_64")) {
            setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        } else {
            setEGLConfigChooser(new MultisampleConfigChooser());
        }

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
