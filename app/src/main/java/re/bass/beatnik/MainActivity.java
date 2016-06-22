package re.bass.beatnik;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Microphone microphone = new Microphone();
        microphone.run();
    }

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("beatnik");
    }
}
