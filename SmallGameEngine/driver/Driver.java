package driver;

import java.awt.BufferCapabilities.FlipContents;
import java.awt.DisplayMode;
import java.awt.EventQueue;
import java.awt.Rectangle;
import java.lang.reflect.InvocationTargetException;
import gamestate.GameState;
import gamegui.GameGUI;

/**
 * Contains the Central Game Loop and relevant static members.
 * @author Nemmer
 */
public class Driver {

	/**
	 * The current game state.
	 */
	private static GameState _gameState;
	/**
	 * The interval of time between two successive updates in the central game loop (in nanoseconds).
	 */
	private static long UPDT_INTERVAL;
	/**
	 * The interval of time between two successive renders in the central game loop (in nanoseconds).
	 */
	private static long REND_INTERVAL;
	/**
	 * Lag sustained between updates.
	 */
	private static long _updtLag;
	/**
	 * Previous update time.
	 */
	private static long _prevUpdtTime;
	/**
	 * Run state.
	 */
	private static boolean _run;
	/**
	 * Game pause state.
	 */
	private static boolean _pause;
	/**
	 * Prints achieved frame rate every second if true for benchmarking.
	 */
	private static boolean _printAchievedFrameRate;
	/**
	 * Prints achieved logical update rate every second if true for benchmarking.
	 */
	private static boolean _printAchievedUpdtRate;
	/**
	 * A constant value to designate the "reasonable" maximum fps (200.0 fps).
	 * Any specified fps higher than this value will be truncated or limited to it.
	 */
	public static final float MAX_FPS = 200.0f;
	
	/**
	 * Creates a Central Game Loop Driver that will be started with the given state.  The Driver will also generate a GameWindow
	 * and display it in windowed mode.  NOTE: Must call start() to begin the execution of the central game loop.
	 * @param initialState The initial game state.
	 * The set GameState object will have its processInput() and updateLogic() methods called
	 * by the central game loop in regular intervals.  Also, its render() method will be called by the central GameWindow object
	 * in regular intervals specified by the Central Game Loop.
	 * @param winBounds The initial bounds of the GameWindow (in screen dimensions).
	 * @param resizable Sets the resizable (by user) state of the GameWindow.
	 * @param bufferSwapMeth The buffer swapping method that should be used to display back buffer data as front buffer
	 * data. It is not guaranteed that the input buffer swap method will be the one used.  The suggested method may not be
	 * available given the current settings on the current system.  The closest method to the one specified will be used.
	 * One can always probe the available buffer swap modes by getting a GameWindow reference and calling the
	 * getBufferCapabilities() function on it before calling start() on the Driver instance (pass null for true blt).
	 */
	public Driver(final GameState initialState, final Rectangle winBounds, final boolean resizable,
						final FlipContents bufferSwapMeth)
	{
		initDriver(initialState, winBounds, resizable, false, null, bufferSwapMeth);
	}
	
	/**
	 * Creates a Central Game Loop Driver that will be started with the given GameState.
	 * The Driver will also generate and display a GameWindow in full screen mode.
	 * Note that unless the FPS or UPS values are modified for the central game loop using setFPS() or setUPS() methods
	 * accordingly, then the central game loop will run with an FPS of 60, and a UPS of 30.
	 * NOTE: Must call start() to begin the execution of the central game loop.
	 * @param initialState The initial game state.
	 * The set GameState object will have its processInput() and updateLogic() methods called
	 * by the central game loop in regular intervals.  Also, its render() method will be called by the central GameWindow object
	 * in regular intervals specified by the Central Game Loop.
	 * @param fullScreenDisplayMode Used to set the preferred display mode for the GameWindow if fullScreen is set true.
	 * 												 The GameWindow will try its best to match the specifications, but on the event that
	 * 												 This device does not have a display mode that meets all of the specifications in the
	 * 												 fullScreenDisplayMode argument, the GameWindow will treat with greatest importance
	 * 												 first the width and height portions, then the refresh rate, and finally the bit depth.
	 * 												 Also, note that the refresh rate and bit depths act as minimum arguments.  Additionally,
	 * 												 if there are no matches for the width and height portions or if this argument is null while
	 * 												 fullScreen is true, then the display mode will be set to the largest possible one on the
	 * 												 present display with a refresh rate of 60hz and a bit-depth of 32 (OPTIONAL -
	 * 												 can just be set null).
	 * @param bufferSwapMeth The buffer swapping method that should be used to display back buffer data as front buffer
	 * data. It is not guaranteed that the input buffer swap method will be the one used.  The suggested method may not be
	 * available given the current settings on the current system.  The closest method to the one specified will be used.
	 * One can always probe the available buffer swap modes by getting a GameWindow reference and calling the
	 * getBufferCapabilities() function on it before calling start() on the Driver instance (pass null for true blt).
	 */
	public Driver(final GameState initialState, final DisplayMode fullScreenDisplayMode,
						final FlipContents bufferSwapMeth)
	{
		initDriver(initialState, null, false, true, fullScreenDisplayMode, bufferSwapMeth);
	}
	
	private void initDriver(final GameState initialState, final Rectangle winBounds, final boolean resizable,
									 final boolean fullScreen, final DisplayMode fullScreenDisplayMode,
									 final FlipContents bufferSwapMeth)
	{
		System.setProperty("sun.awt.noerasebackground", "true");
		setFPS(60);
		setUPS(30);
		_printAchievedFrameRate = false;
		_printAchievedUpdtRate = false;
		_gameState = initialState;
		try {
			EventQueue.invokeAndWait(
					new Runnable() {
						@Override public void run() {
							new GameGUI(winBounds, fullScreen, resizable, fullScreenDisplayMode, bufferSwapMeth);
						}
					});
		}
		catch (InvocationTargetException | InterruptedException e) {  e.printStackTrace();  }
		highResTimerForce();
	}
	
	/**
	 * Starts execution of the central game loop, but first calls the init() method for the set GameState object.
	 */
	public void start() {
		_gameState.init();
		_pause = false;
		new Thread() {
		    public void run() {
		    	Driver.run();
		    }
		}.start();
	}
	
	/**
	 * Returns a reference to the currently set static GameState object.
	 */
	public static GameState getGameState() {
		return _gameState;
	}
	
	/**
	 * Sets the static GameState object.  The set GameState object will have its processInput() and updateLogic() methods called
	 * by the central game loop in regular intervals.  Also, its render() method will be called by the central GameWindow object
	 * in regular intervals specified by the Central Game Loop.
	 * @param gameState The game state to be set.
	 */
	public static void setGameState(GameState gameState) {
		_gameState = gameState;
	}
	
	/**
	 * Stops the Central Game Loop and inevitably leads to program termination.
	 */
	public static void stop() {
		_run = false;
	}
	
	/**
	 * Sets the static pause flag.  If set to true, then the Central Game Loop will pause execution (and sleep in at least 1ms
	 * intervals) until the flag is set false.  (By default, the static pause flag is initially set to false)
	 */
	public static void pause(boolean flag) {
		_pause = flag;
	}
	
	/**
	 * Dynamically sets the frame rate of the game.  If the frame rate is set beyond MAX_FPS (200.0 fps), then it is
	 * truncated or limited to MAX_FPS.
	 * @param fps The number of frames per second.
	 */
	public static void setFPS(float fps) {
		if(fps > MAX_FPS)
			fps = MAX_FPS;
		
		if(fps == 0)
			REND_INTERVAL = Long.MAX_VALUE;
		else
			REND_INTERVAL = (long)((1.0f / fps) * 1000000000.0f);
	}
	
	/**
	 * Sets the loop to print the achieved frame rate each second if flag is set true.
	 * @param flag true to print, false to not print
	 */
	public static void printAchievedFrameRate(boolean flag) {
		_printAchievedFrameRate = flag;
	}
	
	/**
	 * Sets the loop to print the achieved logical update rate each second if flag is set true.
	 * @param flag true to print, false to not print
	 */
	public static void printAchievedUpdtRate(boolean flag) {
		_printAchievedUpdtRate = flag;
	}
	
	/**
	 * Dynamically sets the logical update rate of the game.
	 * @param updateInterval The number of logical updates per second.
	 */
	public static void setUPS(float ups) {
		long currentTime = 0;
		float prevUpdtRatio = 0.0f;
		float lagRatio = 0.0f;
		if(UPDT_INTERVAL != 0) {
			lagRatio = (float)_updtLag / (float)UPDT_INTERVAL;
			prevUpdtRatio = (float)(_prevUpdtTime - (currentTime = System.nanoTime())) / (float)UPDT_INTERVAL;
		}
		if(ups == 0)
			UPDT_INTERVAL = Long.MAX_VALUE;
		else
			UPDT_INTERVAL = (long)((1.0f / ups) * 1000000000.0f);
		_updtLag = (long)(lagRatio * UPDT_INTERVAL);
		_prevUpdtTime = (long)((prevUpdtRatio * UPDT_INTERVAL) + currentTime);
	}
	
	/**
	 * Gets the current UPS specified in the central game loop.
	 * @return The current UPS (logical updates per second).
	 */
	public static float getUPS() {
		return 1.0f / ((float)UPDT_INTERVAL / 1000000000.0f);
	}
	
	/**
	 * Gets the current FPS specified in the central game loop.
	 * @return The current FPS (frames / graphical updates per second).
	 */
	public static float getFPS() {
		return 1.0f / ((float)REND_INTERVAL / 1000000000.0f);
	}
	
	/**
	 * Returns the run state of the driver.
	 * @return true if the central game loop is currently executing, false if not
	 */
	public static boolean isRunning() {
		return _run;
	}
	
	/**
	 * Central game loop method.
	 */
	private static void run() {
		long currentTime, prevRendTime = System.nanoTime(), lastSec = System.nanoTime();
		int numRends = 0, numUpdts = 0;
		_prevUpdtTime = System.nanoTime();
		_updtLag = 0;
		_run = true;
		
		while(_run) {
//			Handle strict pause of game loop.
			if(_pause) {
				currentTime = System.nanoTime();
				do {
					try {  Thread.sleep(1);  }
					catch (InterruptedException e) {  e.printStackTrace();  }
				} while(_pause);
				_prevUpdtTime += System.nanoTime() - currentTime;
			}
						
			_gameState.processInput();
			
//			Update game if it is time (and keep updating it if updates are behind).
			currentTime = System.nanoTime();
			_updtLag += (currentTime - _prevUpdtTime);
			_prevUpdtTime = currentTime;
			while(_updtLag >= UPDT_INTERVAL) { // Play catch-up if needed on slower systems...
				_gameState.updateLogic();
				_updtLag -= UPDT_INTERVAL;
				if(_printAchievedUpdtRate)
					numUpdts++;
			}
			
//			Wait to Render as long as we haven't exceeded REND_INTERVAL and UPDT_INTERVAL.
			currentTime = System.nanoTime();
			while(currentTime - prevRendTime < REND_INTERVAL - 1000000
					 && currentTime - _prevUpdtTime + _updtLag < UPDT_INTERVAL - 1000000)
			{
				try {  Thread.sleep(1);  }
				catch (InterruptedException e) {  e.printStackTrace();  }
				currentTime = System.nanoTime();
			}
			currentTime = System.nanoTime();
			while(currentTime - prevRendTime < REND_INTERVAL
					 && currentTime - _prevUpdtTime + _updtLag < UPDT_INTERVAL)
			{
				Thread.yield();
				currentTime = System.nanoTime();
			}
			
//			Render with interpolation value.
			prevRendTime = System.nanoTime();
			GameGUI.render((float)(prevRendTime - _prevUpdtTime + _updtLag) / (float)UPDT_INTERVAL);
			
//			Print out performance specs after a second has elapsed.
			if(_printAchievedFrameRate) {
				if(currentTime - lastSec >= 1000000000) {
					System.out.println("Achieved FPS: " + (++numRends));
					numRends = 0;
					if(!_printAchievedUpdtRate)
						lastSec = System.nanoTime();
				}
				else
					numRends++;
			}
			if(_printAchievedUpdtRate && currentTime - lastSec >= 1000000000) {
				System.out.println("Achieved UPS: " + numUpdts);
				numUpdts = 0;
				lastSec = System.nanoTime();
			}
		}
		GameGUI.gameWindow().closeWindow();
		System.exit(0);
	}
	
	/**
	 * Attempts to force Java into using a high resolution timer for Thread.Sleep() calls...
	 */
	private static void highResTimerForce() {
		new Thread() {
		    public void run() {
		        try{
		            Thread.sleep(Long.MAX_VALUE);
		        }
		        catch(Exception exc) {}
		    }
		}.start();
	}
}