package re.bass.beatnik.utils;

import android.support.annotation.NonNull;

import java.util.Locale;

/**
 * Created by curly on 7/1/16.
 */

public class CircularFloatBuffer
{
    private final float[] buffer;
    private final int capacity;

    private int head;

    public CircularFloatBuffer(int capacity) {
        if (capacity < 1) {
            throw new IllegalArgumentException(
                    String.format(
                            Locale.getDefault(),
                            "Invalid capacity: %d, must be >0.", capacity
                    )
            );
        }

        this.capacity = capacity;
        buffer = new float[this.capacity];

        head = 0;
    }

    public void append(float value) {
        buffer[head] = value;
        head = (head + 1) % capacity;
    }

    public void appendArray(@NonNull float[] array) {
        int arrayStart = Math.max(0, array.length - capacity);
        int frontLength = Math.min(capacity - head, array.length);

        copyFloats(
                array,
                arrayStart,
                head,
                frontLength
        );
        head += frontLength;

        if (head == buffer.length) {
            int backLength = array.length - (arrayStart + frontLength);
            copyFloats(
                    array,
                    array.length - backLength,
                    0,
                    backLength
            );
            head = backLength;
        }
    }

    public void appendArray(@NonNull double[] array) {
        int arrayStart = Math.max(0, array.length - capacity);
        int frontLength = Math.min(capacity - head, array.length);

        copyDoubles(
                array,
                arrayStart,
                head,
                frontLength
        );
        head += frontLength;

        if (head == buffer.length) {
            int backLength = array.length - (arrayStart + frontLength);
            copyDoubles(
                    array,
                    array.length - backLength,
                    0,
                    backLength
            );
            head = backLength;
        }
    }

    public void copyFrontTo(@NonNull float[] out) {
        if (out.length >= capacity) {
            System.arraycopy(buffer, head, out, 0, capacity - head);
            System.arraycopy(buffer, 0, out, capacity - head, head);
        } else if (out.length <= head) {
            System.arraycopy(
                    buffer,
                    head - out.length,
                    out,
                    0,
                    out.length
            );
        } else {
            System.arraycopy(buffer, 0, out, out.length - head, head);
            System.arraycopy(
                    buffer,
                    capacity + head - out.length,
                    out,
                    0,
                    out.length - head
            );
        }
    }

    private void copyFloats(
            float[] floats,
            int inputStart,
            int outputStart,
            int length
    ) {
        System.arraycopy(
                floats,
                inputStart,
                buffer,
                outputStart,
                length
        );
    }

    private void copyDoubles(
            double[] doubles,
            int inputStart,
            int outputStart,
            int length
    ) {
        for (int i = 0; i < length; i++) {
            buffer[outputStart + i] = (float) doubles[inputStart + i];
        }
    }

    public float[] toArray() {
        return toArray(capacity);
    }

    public float[] toArray(int length) {
        if (length < 1) {
            throw new IllegalArgumentException("Invalid array length.");
        }
        float[] array = new float[length];
        copyFrontTo(array);

        return array;
    }
}