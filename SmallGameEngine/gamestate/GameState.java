package gamestate;

import gamegui.GameGUI.GameWindow;
import gamegui.GameGUI;
import gamegui.GameWindowRenderer;

/**
 * Holds state information pertaining to the current active portion of the game.
 * @author Nemmer
 */
public abstract class GameState implements GameWindowRenderer {
	
	protected GameWindow _window;
	protected boolean _pause, _isInit = false;
	
	/**
	 * Default constructor (must later call init() to initialize original values).
	 */
	public GameState() {}
	
	/**
	 * Initializes the original values for the Game State instance.
	 */
	public void init() {
		_window = GameGUI.gameWindow();
		_pause = false;
		_isInit = true;
	}
	
	/**
	 * Returns true if the init() function was called for this GameState object, false if it was not called.
	 */
	public boolean isInit() {
		return _isInit;
	}
	
	/**
	 * Sets the pause state.
	 * @param pause true if paused, false if not paused.
	 */
	public void setPause(boolean pause) {
		_pause = pause;
	}
	
	/**
	 * Returns whether or not the game is paused.
	 * @return true if paused, false if not paused.
	 */
	public boolean isPaused() {
		return _pause;
	}
	
	/**
	 * Called by central game loop to schedule this game state's input processing.
	 */
	public abstract void processInput();
	/**
	 * Called by central game loop to schedule a logical update for this game state.
	 */
	public abstract void updateLogic();
}