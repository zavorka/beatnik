package re.bass.beatnik.plot;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.util.AttributeSet;
import android.util.Log;

import java.nio.FloatBuffer;

import re.bass.beatnik.utils.MultisampleConfigChooser;

// Copyright (c) 2016 Roman Beránek. All rights reserved.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
