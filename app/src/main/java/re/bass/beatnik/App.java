package re.bass.beatnik;

import android.app.Application;
import android.content.Context;

public class App extends Application {

	private static App app;

	public static App get() {
		return app;
	}

	public String string(int resId) {
		return app.getResources().getString(resId);
	}

	public static Context context() {
		return app.getBaseContext();
	}

	@Override public void onCreate() {
		super.onCreate();
		app = this;
	}
}
