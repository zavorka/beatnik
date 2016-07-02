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

        System.arraycopy(
                array,
                arrayStart,
                buffer,
                head,
                frontLength
        );
        head += frontLength;

        if (frontLength < buffer.length) {
            int backLength = array.length - (arrayStart + frontLength);
            System.arraycopy(
                    array,
                    array.length - backLength,
                    buffer,
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

    public class ArrayTooLargeException extends Exception {

    }
}