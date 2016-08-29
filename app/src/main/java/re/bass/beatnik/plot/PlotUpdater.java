package re.bass.beatnik.plot;

import android.opengl.GLSurfaceView;

import java.util.ArrayList;
import java.util.List;

import re.bass.beatnik.Startable;

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

public class PlotUpdater implements Startable, Runnable
{
    final private List<GLSurfaceView> views = new ArrayList<>();
    Thread thread;
    boolean running = false;

    public PlotUpdater() {
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
        synchronized (this) {
            notify();
        }
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
