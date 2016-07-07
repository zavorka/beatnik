package re.bass.beatnik.utils;

import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import static org.hamcrest.Matchers.equalTo;
import static org.junit.Assert.*;


/**
 * Created by curly on 7/7/16.
 */

public class CircularBufferTest {
    private static final int CAPACITY = 8;

    @Test
    public void newBufferIsFilledWithZeros() {
        CircularFloatBuffer buffer = new CircularFloatBuffer(CAPACITY);

        float[] copy = new float[CAPACITY];
        buffer.copyFrontTo(copy);
        for (float value : copy) {
            assertThat(value, equalTo(0.0f));
        }
    }

    @Test
    public void keepsCorrectOrder() {
        CircularFloatBuffer buffer = new CircularFloatBuffer(CAPACITY);

        float[] copy = new float[CAPACITY];
        float[] sequence = new float[CAPACITY];

        for (int i = 0; i < CAPACITY; i++) {
            buffer.append(i);
            sequence[i] = i;
        }
        buffer.copyFrontTo(copy);

        assertThat(copy, equalTo(sequence));
    }

    @Test
    public void copyIntoSmallerArray() {
        CircularFloatBuffer buffer = new CircularFloatBuffer(8);
        buffer.append(0);
        buffer.append(1);
        buffer.append(2);
        buffer.append(3);
        buffer.append(4);
        buffer.append(5);
        buffer.append(6);
        buffer.append(7);

        float[] copy = new float[4];
        buffer.copyFrontTo(copy);

        assertThat(copy, equalTo(new float[] { 4, 5, 6, 7 }));
    }

    @Test
    public void isCircular() {
        CircularFloatBuffer buffer = new CircularFloatBuffer(3);
        buffer.append(0);
        buffer.append(1);
        buffer.append(2);
        buffer.append(3);
        buffer.append(4);
        buffer.append(5);
        buffer.append(6);
        buffer.append(7);

        float[] copy = new float[3];
        buffer.copyFrontTo(copy);

        assertThat(copy, equalTo(new float[] { 5, 6, 7 }));
    }

    @Test
    public void partiallyFillsLargerArrays() {
        CircularFloatBuffer buffer = new CircularFloatBuffer(3);
        buffer.append(1);
        buffer.append(1);
        buffer.append(1);

        float[] copy = new float[] { 0, 1, 2, 3, 4 };
        buffer.copyFrontTo(copy);

        assertThat(copy, equalTo(new float[] { 1, 1, 1, 3, 4 }));
    }

    @Test
    public void appendsFloatArrays() {
        CircularFloatBuffer buffer = new CircularFloatBuffer(5);
        buffer.appendArray(new float[] { 0, 1, 2 });
        buffer.appendArray(new float[] { 4, 4 });

        float[] copy = new float[5];
        buffer.copyFrontTo(copy);

        assertThat(copy, equalTo(new float[] { 0, 1, 2, 4, 4 }));
    }

    @Test
    public void appendsArraysLargerThanItself() {
        CircularFloatBuffer buffer = new CircularFloatBuffer(3);
        buffer.appendArray(new float[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 });

        float[] copy = new float[3];
        buffer.copyFrontTo(copy);

        assertThat(copy, equalTo(new float[] { 7, 8, 9 }));
    }

    @Test
    public void appendsDoubleArrays() {
        CircularFloatBuffer buffer = new CircularFloatBuffer(3);
        float[] floatArray = new float[] { 0, 1, 2 };
        double[] doubleArray = new double[] { 0, 1, 2 };

        buffer.appendArray(doubleArray);
        float[] copy = new float[3];
        buffer.copyFrontTo(copy);

        assertThat(copy, equalTo(floatArray));
    }

    @Test
    public void createsItsArrayCopy() {
        CircularFloatBuffer buffer = new CircularFloatBuffer(4);
        buffer.append(0);
        buffer.append(1);
        buffer.append(2);
        buffer.append(3);

        assertThat(buffer.toArray(), equalTo(new float[] { 0, 1, 2, 3 }));
    }

    @Test
    public void createsItsShorterArrayCopy() {
        CircularFloatBuffer buffer = new CircularFloatBuffer(4);
        buffer.append(0);
        buffer.append(1);
        buffer.append(2);
        buffer.append(3);

        assertThat(buffer.toArray(2), equalTo(new float[] { 2, 3}));
    }

    @Test (expected = IllegalArgumentException.class)
    public void checksForValidCapacity() {
        new CircularFloatBuffer(0);
    }

    @Test (expected = IllegalArgumentException.class)
    public void checksForValidArrayLength() {
        CircularFloatBuffer buffer = new CircularFloatBuffer(3);
        buffer.toArray(0);
    }
}
