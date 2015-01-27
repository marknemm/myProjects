package gamegui.buttons;

import gamegui.GameWindowComponent;
import java.awt.Color;
import java.awt.Font;
import java.awt.Rectangle;
import java.awt.event.MouseListener;
import java.awt.image.BufferedImage;

/**
 * An Abstract Class to model all custom made buttons off of.
 * @author Nemmer
 */
public abstract class GameButton implements GameWindowComponent, MouseListener {

	protected enum State {
		NORMAL(0), HOVER(1), PRESSED(2);
		private final int _stateInt;
		private State(int stateInt) {  _stateInt = stateInt;  }
		public int getStateInt() {  return _stateInt;  }
	}
	
	protected GameButtonListener _listener;
	protected Rectangle _bounds;
	protected String _text;
	protected State _state;
	protected Font _font;
	protected BufferedImage[] _buttonImgs;
	protected Color[] _buttonColors;
	protected Color _textColor;
	
	public GameButton(Rectangle bounds, String text) {
		_bounds = bounds;
		_text = text;
		_state = State.NORMAL;
		_font = new Font("Arial", Font.PLAIN, 12);
		_textColor = Color.BLACK;
		_buttonColors = new Color[3];
		_buttonImgs = new BufferedImage[3];
		_buttonColors[State.NORMAL.getStateInt()] = Color.BLUE;
		_buttonColors[State.HOVER.getStateInt()] = new Color(100, 100, 255);
		_buttonColors[State.PRESSED.getStateInt()] = Color.CYAN;
	}
	
	public void setFont(Font font) {
		_font = font;
	}
	
	public void setTextColor(Color color) {
		_textColor = color;
	}
	
	public void setText(String text) {
		_text = text;
	}
	
	public void setButtonColor(Color color) {
		_buttonColors[State.NORMAL.getStateInt()] = color;
	}
	
	public void setHoverButtonColor(Color color) {
		_buttonColors[State.HOVER.getStateInt()] = color;
	}
	
	public void setPressedButtonColor(Color color) {
		_buttonColors[State.PRESSED.getStateInt()] = color;
	}
	
	public void setGameButtonListener(GameButtonListener listener) {
		_listener = listener;
	}
	
	public void setNormalButtonImg(BufferedImage buttonImg) {
		_buttonImgs[State.NORMAL.getStateInt()] = buttonImg;
	}
	
	public void setHoverButtonImg(BufferedImage buttonImg) {
		_buttonImgs[State.HOVER.getStateInt()] = buttonImg;
	}
	
	public void setPressedButtonImg(BufferedImage buttonImg) {
		_buttonImgs[State.PRESSED.getStateInt()] = buttonImg;
	}
	
	public abstract void processInput();
}