package gamegui.buttons;

public class GameButtonEvent {
	
	public final static int NONE = 0;
	public final static int PRESSED = 1;
	public final static int MOUSE_ENTER_REGION = 2;
	public final static int MOUSE_EXIT_REGION = 3;
	public final static int RELEASED_IN_REGION = 4;
	public final static int RELEASED_OUT_REGION = 5;
	private int _event;
	
	public GameButtonEvent() {
		_event = NONE;
	}
	
	public int getEvent() {
		return _event;
	}
	
	public void setEvent(int event) {
		_event = event;
	}
}