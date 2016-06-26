package re.bass.beatnik.tool;

import android.graphics.Typeface;
import android.support.v4.util.SimpleArrayMap;

import re.bass.beatnik.App;

public class FontManager {

	private static final SimpleArrayMap<String, Typeface> cache = new SimpleArrayMap();

	public static String MUNRO = "Munro";
	public static String MUNRO_SMALL = "MunroSmall";

	public static Typeface get(String name) {
		synchronized(cache) {
			if(!cache.containsKey(name)) {
				Typeface t = Typeface.createFromAsset(App.get().getAssets(), String.format("fonts/%s.ttf", new Object[]{name}));
				cache.put(name, t);
				return t;
			} else {
				return cache.get(name);
			}
		}
	}
}
