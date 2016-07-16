package re.bass.beatnik.plot;

import android.opengl.GLES20;
import android.opengl.GLSurfaceView;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by curly on 7/8/16.
 */

@SuppressWarnings("FieldCanBeLocal")
public class PlotRenderer implements GLSurfaceView.Renderer
{
    private static final String VERTEX_SHADER_CODE = ""
            + "attribute float x; "
            + "attribute float y; "
            + ""
            + "void main(void) { "
            + "    gl_Position = vec4(x * 2.0 - 1.0, y * 2.0 - 1.0, 0.0, 1.0); "
            + "}";

    private static final String FRAGMENT_SHADER_CODE = ""
            + "precision mediump float; "
            + "uniform vec4 vColor; "
            + "void main() { "
            + "    gl_FragColor = vColor; "
            + "}";

    private enum ShaderType {
        VERTEX_SHADER,
        FRAGMENT_SHADER
    }

    private static final int STRIDE = (Float.SIZE / Byte.SIZE);
    private static final float[] WHITE = new float[] { 1.0f, 1.0f, 1.0f, 1.0f };

    private final int pointsCount;
    private final float[] points;
    private final float[] scaledPoints;

    private final FloatBuffer xBuffer;
    private int xHandle;
    private final FloatBuffer yBuffer;
    private int yHandle;

    private int colorHandle;

    private int program;

    private int width;
    private int height;

    public PlotRenderer(int pointsCount) {
        this.pointsCount = pointsCount;
        points = new float[this.pointsCount];
        scaledPoints = new float[this.pointsCount];

        ByteBuffer buffer = ByteBuffer
                .allocateDirect(pointsCount * STRIDE);
        buffer.order(ByteOrder.nativeOrder());
        xBuffer = buffer.asFloatBuffer();

        buffer = ByteBuffer
                .allocateDirect(pointsCount * STRIDE);
        buffer.order(ByteOrder.nativeOrder());
        yBuffer = buffer.asFloatBuffer();

        initXBuffer();
    }

    public void initialize() {
        program = GLES20.glCreateProgram();
        GLES20.glAttachShader(
                program,
                loadShader(
                        ShaderType.VERTEX_SHADER,
                        VERTEX_SHADER_CODE
                )
        );
        GLES20.glAttachShader(
                program,
                loadShader(
                        ShaderType.FRAGMENT_SHADER,
                        FRAGMENT_SHADER_CODE
                )
        );
        GLES20.glLinkProgram(program);
    }

    public FloatBuffer getPlotBuffer() {
        return yBuffer;
    }

    private void initXBuffer() {
        for (int i = 0; i < pointsCount; i++) {
            xBuffer.put((float) i / (float) (pointsCount - 1));
        }
        xBuffer.position(0);
    }

    public void setPoints(final float[] points) {
        synchronized (yBuffer) {
            yBuffer.put(points);
            yBuffer.position(0);
        }
    }

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        GLES20.glClearColor(1.0f, 87f/256f, 34f/256f, 1.0f);
        initialize();
    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int width, int height) {
        GLES20.glViewport(0, 0, width, height);
        this.width = width;
        this.height = height;
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);

        GLES20.glUseProgram(program);
        xHandle = GLES20.glGetAttribLocation(program, "x");
        GLES20.glEnableVertexAttribArray(xHandle);
        GLES20.glVertexAttribPointer(
                xHandle,
                1,
                GLES20.GL_FLOAT,
                false,
                0,
                xBuffer
        );
        yHandle = GLES20.glGetAttribLocation(program, "y");
        GLES20.glEnableVertexAttribArray(yHandle);
        GLES20.glVertexAttribPointer(
                yHandle,
                1,
                GLES20.GL_FLOAT,
                false,
                0,
                yBuffer
        );

        colorHandle = GLES20.glGetUniformLocation(program, "vColor");
        GLES20.glUniform4fv(colorHandle, 1, WHITE, 0);

        GLES20.glLineWidth(5.0f);

        synchronized (this) {
            GLES20.glDrawArrays(GLES20.GL_LINE_STRIP, 0, pointsCount);
        }

        GLES20.glDisableVertexAttribArray(xHandle);
        GLES20.glDisableVertexAttribArray(yHandle);
    }

    private static int loadShader(ShaderType type, String code) {
        int typeValue = 0;
        switch (type) {
            case VERTEX_SHADER:
                typeValue = GLES20.GL_VERTEX_SHADER;
                break;
            case FRAGMENT_SHADER:
                typeValue = GLES20.GL_FRAGMENT_SHADER;
                break;
        }

        int shader = GLES20.glCreateShader(typeValue);

        // add the source code to the shader and compile it
        GLES20.glShaderSource(shader, code);
        GLES20.glCompileShader(shader);

        return shader;
    }
}
