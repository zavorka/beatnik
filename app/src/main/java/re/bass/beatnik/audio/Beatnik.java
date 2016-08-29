package re.bass.beatnik.audio;

import java.nio.ShortBuffer;
import java.nio.FloatBuffer;

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

public class Beatnik {
    public static native void clear();
    public static native boolean process(final ShortBuffer input);
    public static native float getCurrentTempo();
    public static native void attachFFTPlotBuffer(FloatBuffer plot);
    public static native int getRequiredSampleRate();
    public static native int getRequiredStepSize();
}
