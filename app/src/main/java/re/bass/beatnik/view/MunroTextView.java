package re.bass.beatnik.view;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.TextView;

import re.bass.beatnik.tool.FontManager;

public class MunroTextView extends TextView {

	public MunroTextView(Context context) {
		super(context);
		setFont();
	}

	public MunroTextView(Context context, AttributeSet attrs) {
		super(context, attrs);
		setFont();
	}

	public MunroTextView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		setFont();
	}

	private void setFont() {
		if (!this.isInEditMode()) {
			setTypeface(FontManager.get(FontManager.MUNRO));
		}
	}
}
