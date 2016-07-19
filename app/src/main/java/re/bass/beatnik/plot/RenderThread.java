package re.bass.beatnik.plot;

import android.opengl.GLSurfaceView;

import java.util.ArrayList;
import java.util.List;

import re.bass.beatnik.Startable;

/**
 * Created by curly on 7/19/16.
 */

public class RenderThread implements Startable, Runnable
{
    final private List<GLSurfaceView> views = new ArrayList<>();
    Thread thread;
    boolean running = false;

    public RenderThread() {
        super();
    }

    public void addGlSurfaceView(GLSurfaceView view) {
        synchronized (views) {
            views.add(view);
        }
    }

    public void removeSurfaceView(GLSurfaceView view) {
        synchronized (views) {
            views.remove(view);
        }
    }

    @Override
    public void start() {
        running = true;
        thread = new Thread(this);
        thread.start();
    }

    public void stop() {
        running = false;
        try {
            thread.join();
        } catch (InterruptedException e) {
            thread.interrupt();
        }
    }

    public void requestRender() {
        synchronized (this) {
            notify();
        }
    }

    @Override
    public void run() {
        while (running) {
            try {
                synchronized (this) {
                    wait();
                }
            } catch (InterruptedException e) {
                thread.interrupt();
            }
            synchronized (views) {
                for (GLSurfaceView view : views) {
                    view.requestRender();
                }
            }
        }
    }

}
