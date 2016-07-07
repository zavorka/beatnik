package re.bass.beatnik.plot;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.PorterDuff;
import android.os.Process;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import re.bass.beatnik.Stoppable;

/**
 * Created by curly on 01/07/2016.
 */

public abstract class PlotView extends SurfaceView
        implements
        SurfaceHolder.Callback,
        Runnable,
        Stoppable
{
    private static final String TAG = "PlotView";
    private static final int PIXELS_PER_LINE = 4;
    private static final int MAX_POINTS = 512;

    protected float[] plotBuffer;
    private float[] lines;
    private float[] points;

    private Thread thread;
    private boolean running = false;
    private boolean initialized = false;

    private boolean needRedraw;
    protected Paint paint;

    private SurfaceHolder holder;

    public PlotView(Context context) {
        super(context);
        initialize();
    }

    public PlotView(
            Context context,
            AttributeSet attrs
    ) {
        super(context, attrs);
        initialize();
    }

    public PlotView(
            Context context,
            AttributeSet attrs,
            int defStyleAttr
    ) {
        super(context, attrs, defStyleAttr);
        initialize();
    }

    protected void initialize() {
        if (initialized) {
            return;
        }

        paint = new Paint();
        paint.setColor(Color.WHITE);
        paint.setAntiAlias(true);
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(5f);

        setZOrderOnTop(true);
        getHolder().setFormat(PixelFormat.TRANSPARENT);
        getHolder().addCallback(this);

        initialized = true;
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.v(TAG, "surfaceCreated()");
        this.holder = holder;
    }

    @Override
    public void surfaceChanged(
            SurfaceHolder holder,
            int format,
            int width,
            int height
    ) {
        Log.v(TAG, String.format(
                "surfaceChanged(holder, %d, %d, %d)",
                format,
                width,
                height
        ));
        stop();

        int numberOfPoints = width / PIXELS_PER_LINE;
        int nextPower = 1;
        while (nextPower < numberOfPoints) {
            nextPower <<= 1;
        }
        int prevPower = nextPower / 2;
        if ((numberOfPoints - prevPower) < (nextPower - numberOfPoints)) {
            numberOfPoints = prevPower;
        } else {
            numberOfPoints = nextPower;
        }

        if (numberOfPoints > MAX_POINTS) {
            numberOfPoints = MAX_POINTS;
        }

        lines = new float[(numberOfPoints - 1) * 4];
        points = new float[numberOfPoints];

        start();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.v(TAG, "surfaceDestroyed()");
        stop();
    }

    private void start() {
        if (running) {
            return;
        }
        running = true;
        thread = new Thread(this);
        thread.start();
    }

    @Override
    public void stop() {
        if (!running) {
            return;
        }

        running = false;
        Log.v(TAG, "running = false");
        boolean retry = true;
        while (retry) {
            try {
                thread.join();
                retry = false;
            } catch (InterruptedException e) {
                // try again shutting down the thread
            }
        }
    }

    @Override
    public void draw(Canvas canvas) {
        canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);

        updateBuffer();

        int width = canvas.getWidth();
        int height = canvas.getHeight();

        synchronized (this) {
            if (plotBuffer == null) {
                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                return;
            }

            updateLines(width, height);
        }

        canvas.drawLines(lines, paint);
        needRedraw = false;
    }

    protected abstract void updateBuffer();

    private void updateLines(int width, int height) {
        int valuesPerPoint = plotBuffer.length / points.length;
        for (int i = 0; i < (points.length * valuesPerPoint); i++) {
            points[i / valuesPerPoint] = plotBuffer[i] / valuesPerPoint;
        }

        //int linesCount = lines.length / 4;
        //float valuesPerLine = plotBuffer.length / (linesCount + 1);

        float xScale = (float) width / (float) points.length;
        float yScale = height * 2;

        for (int i = 1; i < points.length; i++) {
            lines[4 * i - 4] = xScale * (i - 1);
            lines[4 * i - 2] = xScale * (i);

            lines[4 * i - 3] = height
                    - (yScale * plotBuffer[i - 1]);
            lines[4 * i - 1] = height
                    - (yScale * plotBuffer[i]);
        }
    }


    @Override
    public void run() {
        Process.setThreadPriority(Process.THREAD_PRIORITY_BACKGROUND);
        while (running) {
            try {
                Thread.sleep(1);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            updatePlot();
        }
    }

    private void updatePlot() {
        if (!running || !needRedraw || !holder.getSurface().isValid()) {
            return;
        }

        Canvas canvas = holder.lockCanvas();
        if (canvas == null) {
            return;
        }
        draw(canvas);
        holder.unlockCanvasAndPost(canvas);
    }

    protected void dataChanged() {
        needRedraw = true;
    }
}
