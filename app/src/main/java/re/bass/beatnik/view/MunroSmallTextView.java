package re.bass.beatnik.view;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.TextView;

import re.bass.beatnik.tool.FontManager;

public class MunroSmallTextView extends TextView {

	public MunroSmallTextView(Context context) {
		super(context);
		setFont();
	}

	public MunroSmallTextView(Context context, AttributeSet attrs) {
		super(context, attrs);
		setFont();
	}

	public MunroSmallTextView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		setFont();
	}

	private void setFont() {
		if (!this.isInEditMode()) {
			setTypeface(FontManager.get(FontManager.MUNRO_SMALL));
		}
	}
}
