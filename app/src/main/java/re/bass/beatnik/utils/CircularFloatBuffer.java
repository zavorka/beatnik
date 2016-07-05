package re.bass.beatnik.utils;

/**
 * Created by curly on 7/1/16.
 */

public class CircularFloatBuffer
{
    private final float[] buffer;
    private final int capacity;

    private int head;

    public CircularFloatBuffer(int capacity) {
        this.capacity = capacity;
        buffer = new float[this.capacity];

        head = 0;
    }

    public void append(float value) {
        buffer[head] = value;
        head = (head + 1) % capacity;
    }

    public void appendArray(float[] array) {
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

    public void appendArray(double[] array) {
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

    public void copyFrontTo(float[] out) throws ArrayTooLargeException {
        if (out.length > capacity) {
            throw new ArrayTooLargeException();
        }

        System.arraycopy(buffer, head, out, 0, capacity - head);
        System.arraycopy(buffer, 0, out, capacity - head, head);
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

    public class ArrayTooLargeException extends Exception {

    }
}