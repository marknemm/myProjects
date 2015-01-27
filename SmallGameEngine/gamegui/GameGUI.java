package gamegui;

import java.awt.AWTException;
import java.awt.BufferCapabilities;
import java.awt.BufferCapabilities.FlipContents;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.DisplayMode;
import java.awt.GraphicsConfiguration;
import java.awt.GraphicsDevice;
import java.awt.GraphicsEnvironment;
import java.awt.ImageCapabilities;
import java.awt.LayoutManager;
import java.awt.Rectangle;
import java.awt.Toolkit;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.awt.geom.Area;
import java.util.ArrayList;
import driver.Driver;

/**
 * Contains GUI specific code for the game.  Acts primarily as a wrapper around a central static game window (GameWindow).
 * @author Nemmer
 */
public class GameGUI {
	
	private static GameWindow _gameWindow;

	/**
	 * Constructs the GUI portion of the game by creating and initializing a GameWindow.
	 * NOTE: This constructor should not be called outside of the main Driver class of this game engine.
	 * @param winRect The bounding rectangle (on the screen) of the GameWindow (ignored in full screen mode).
	 * @param fullScreen True if window should be in full screen mode, false if it should be in windowed mode.
	 * @param resizable True if the window may be resized by the user, false if it may not.
	 * @param fullScreenDisplayMode The display mode that the window should have in full screen mode (ignored if fullScreen is
	 * false).  If null, then the best guess display mode is chosen (see Driver full screen mode constructor for more details).
	 * @param bufferSwapMeth The buffer swapping method that should be used to display back buffer data as front buffer
	 * data. It is not guaranteed that the input buffer swap method will be the one used.  The suggested method may not be
	 * available given the current settings on the current system.  The closest method to the one specified will be used.
	 * One can also always probe the available buffer swap modes by getting a GameWindow reference and calling the
	 * getBufferCapabilities() function on it before calling start() on the Driver instance.
	 */
	public GameGUI(Rectangle winRect, boolean fullScreen, boolean resizable, DisplayMode fullScreenDisplayMode,
							 FlipContents bufferSwapMeth)
	{
		_gameWindow = new GameWindow(winRect, fullScreen, resizable, fullScreenDisplayMode, bufferSwapMeth);
	}

	/**
	 * Returns a global reference to the central GameWindow.
	 */
	public static GameWindow gameWindow() {
		return _gameWindow;
	}

	/**
	 * Updates the central GameWindow.
	 * NOTE: This function should only be called from the central game loop within the Driver.
	 * @param interpolation The interpolation value (percentage of time passed between last update and next update).
	 */
	public static void render(float interpolation) {
		_gameWindow.render(interpolation);
	}
	
	/**
	 * The status of the GameWindow's windowing mode.
	 * @author Nemmer
	 */
	public enum WindowMode {  FullScreen, SimulatedFullScreen, Windowed  };

	/**
	 * The window we will use to render the entire game to.  Basically wraps a Frame with a Canvas component and related
	 * functionality.
	 * @author Nemmer
	 */
	public class GameWindow implements WindowListener, KeyListener, MouseListener, MouseMotionListener {
	
		private FlipContents _bufferSwapMeth;
		private ArrayList<Component> _components;
		private ArrayList<GameWindowComponent> _gameComponents;
		private ArrayList<MouseListener> _mouseListeners;
		private ArrayList<KeyListener> _keyListeners;
		private ArrayList<MouseMotionListener> _mouseMotionListeners;
		private GameFrame _gameFrame;
		private WindowMode _windowMode;
		private Dimension _prevDim;
		private int _numBuffers;
	
		/**
		 * Initializes a GameWindow object.
		 * @param winRect
		 * @param fullScreen 
		 * @param resizable 
		 * @param fullScreenDisplayMode
		 * @param bufferSwapMethod
		 */
		private GameWindow(Rectangle winRect, boolean fullScreen, boolean resizable, DisplayMode fullScreenDisplayMode,
										FlipContents bufferSwapMeth)
		{
			_bufferSwapMeth = FlipContents.UNDEFINED;
			_numBuffers = 2;
			_prevDim = new Dimension();
			_components = new ArrayList<Component>();
			_gameComponents = new ArrayList<GameWindowComponent>();
			_mouseListeners = new ArrayList<MouseListener>();
			_keyListeners = new ArrayList<KeyListener>();
			_mouseMotionListeners = new ArrayList<MouseMotionListener>();
			_gameFrame = new GameFrame(this);
			_gameFrame.addWindowListener(this);
			_gameFrame.addMouseListener(this);
			_gameFrame.addMouseMotionListener(this);
			if(fullScreen)		initFullScreen(fullScreenDisplayMode);
			else					initNonFullScreen(winRect, resizable);
			_bufferSwapMeth = setBufferSwapMethod(bufferSwapMeth);
		}
		
		private void initFullScreen(DisplayMode fullScreenDisplayMode) {
			_windowMode = WindowMode.FullScreen;
			_gameFrame.setUndecorated(true);
			GraphicsEnvironment ge = GraphicsEnvironment.getLocalGraphicsEnvironment();
			GraphicsDevice screen = ge.getDefaultScreenDevice();
			if(screen.isFullScreenSupported()) {
				screen.setFullScreenWindow(_gameFrame);
				setPreferredDisplayMode(screen, fullScreenDisplayMode);
			}
			else
				simulateFullScreen();
			_gameFrame.addKeyListener(this);
			_gameFrame.requestFocus();
		}
	
		private void setPreferredDisplayMode(GraphicsDevice screen, DisplayMode preferred) {
			if(preferred == null) { // Let engine find best one.
				Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
				preferred = new DisplayMode(screenSize.width, screenSize.height, 32, 60);
			}
			DisplayMode[] displays = screen.getDisplayModes();
			DisplayMode altDisplay = null;
			for(int i = 0 ; i < displays.length ; i++) {
				if(displays[i].getWidth() == preferred.getWidth() && displays[i].getHeight() == preferred.getHeight()) {
					if(displays[i].getBitDepth() >= preferred.getBitDepth()
					&& displays[i].getRefreshRate() >= preferred.getRefreshRate())
					{
						screen.setDisplayMode(displays[i]);
						return;
					}
					else
						altDisplay = displays[i];
				}
			}
			if(altDisplay != null)
				screen.setDisplayMode(altDisplay);
			else
				setPreferredDisplayMode(screen, null); // Engine finds best one now...
		}
	
		private void simulateFullScreen() {
			_windowMode = WindowMode.SimulatedFullScreen;
			_gameFrame.setPreferredSize(Toolkit.getDefaultToolkit().getScreenSize());
			_gameFrame.setLocation(0, 0);
			_gameFrame.pack();
			_gameFrame.setVisible(true);
		}
	
		private void initNonFullScreen(Rectangle winRect, boolean resizable) {
			_windowMode = WindowMode.Windowed;
			_gameFrame.setBounds(winRect);
			_gameFrame.setPreferredSize(winRect.getSize());
			_gameFrame.setUndecorated(false);
			_gameFrame.setResizable(resizable);
			_gameFrame.addKeyListener(this);
			_gameFrame.pack();
			_gameFrame.setVisible(true);
			_gameFrame.requestFocus();
		}
	
		/**
		 * Returns the current window mode that the GameWindow is in.  (FullScreen, SimulatedFullScreen, or Windowed)
		 */
		public WindowMode getWindowMode() {
			return _windowMode;
		}
	
		/**
		 * Sets the GameWindow into full screen mode.  If the GameWindow is already in full screen mode, then this method does
		 * nothing.
		 */
		public void setFullScreenMode() {
			setFullScreenMode(null);
		}
	
		/**
		 * Sets the GameWindow into full screen mode.  The GameWindow will completely
		 * transition as a result of this call if it is not already in full screen mode.  Also, sets the display mode
		 * for the full screen GameWindow based off of the preferredDisplayMode argument.
		 * @param preferredDisplayMode 
		 * Contains the preferred display mode qualities.  The GameWindow will treat with greatest importance
		 * 	first the width and height portions, then the refresh rate, and finally the bit depth.  Also, note that the refresh rate and bit
		 * depths act as minimum arguments.  Additionally, if there are no matches for the width and height portions or if this
		 * argument is null, then the display mode will be set to the largest possible one on the present
		 * device with a refresh rate of 60hz and a bit-depth of 32.
		 */
		public void setFullScreenMode(DisplayMode preferredDisplayMode) {
			if(_windowMode != WindowMode.Windowed)
				return;
			closeWindow(false);
			_gameFrame = new GameFrame(this);
			initFullScreen(preferredDisplayMode);
			transferListeners();
			_gameFrame.addWindowListener(this);
			_gameFrame.addMouseListener(this);
			_gameFrame.addMouseMotionListener(this);
		}
	
		/**
		 * Sets the GameWindow into windowed mode.  If the GameWindow is already in windowed mode, then nothing will happen
		 * as a result of this method call.  All appropriate changes will manifest as a result of a call to this method.
		 * @param bounds The placement and size of the window.  WARNING: If null, then this method will throw a
		 * NullPointerException.
		 * @param resizable A flag to tell whether or not the window will be resizable (by the user).
		 */
		public void setWindowedMode(Rectangle bounds, boolean resizable) {
			if(_windowMode == WindowMode.Windowed)
				return;
			closeWindow(false);
			_gameFrame = new GameFrame(this);
			initNonFullScreen(bounds, resizable);
			transferListeners();
			_gameFrame.addWindowListener(this);
			_gameFrame.addMouseListener(this);
			_gameFrame.addMouseMotionListener(this);
		}
	
		private void transferListeners() {
			int i;
			for(i = 0 ; i < _mouseListeners.size() ; i++)
				_gameFrame.addMouseListener(_mouseListeners.get(i));
		
			for(i = 0; i < _keyListeners.size() ; i++)
				_gameFrame.addKeyListener(_keyListeners.get(i));
		
			for(i = 0 ; i < _mouseMotionListeners.size() ; i++)
				_gameFrame.addMouseMotionListener(_mouseMotionListeners.get(i));
		}
	
		/**
		 * Sets the preferred display mode for the GameWindow in full screen mode.
		 * This method will find the best matching display mode and set it based off
		 * of the specifications given in the preferredDisplayMode argument.
		 * @param preferredDisplayMode The preferred display mode qualities.  The GameWindow will treat with greatest importance
		 * 	first the width and height portions, then the refresh rate, and finally the bit depth.  Also, note that the refresh rate and bit
		 * depths act as minimum arguments.  Additionally, if there are no matches for the width and height portions or if this
		 * argument is null, then the display mode will be set to the largest possible one on the present
		 * display with a refresh rate of 60hz and a bit-depth of 32.
		 * @return The actual display mode that was set (May not match the preferred one if such a DisplayMode isn't available).
		 * Will return null if full screen mode isn't enabled for GameWindow, and the display mode will not be set.
		 */
		public DisplayMode setPreferredDisplayMode(DisplayMode preferredDisplayMode) {
			if(_windowMode == WindowMode.Windowed)
				return null;
			GraphicsEnvironment ge = GraphicsEnvironment.getLocalGraphicsEnvironment();
			GraphicsDevice screen = ge.getDefaultScreenDevice();
			setPreferredDisplayMode(screen, preferredDisplayMode);
			return screen.getDisplayMode();
		}
	
		/**
		 * Prints out all available Display Modes on this device for the GameWindow to be able to use in full screen mode.
		 */
		public void printDisplayModes() {
			GraphicsEnvironment ge = GraphicsEnvironment.getLocalGraphicsEnvironment();
			GraphicsDevice screen = ge.getDefaultScreenDevice();
			DisplayMode[] displays = screen.getDisplayModes();
			System.out.println("--- CURRENTLY AVAILABLE DISPLAY MODES ---");
			for(int i = 0 ; i < displays.length ; i++)
				System.out.println("Display " + i + "'s Width = " + displays[i].getWidth() + "       Height = " + displays[i].getHeight()
				+ "       Bit Depth = " + displays[i].getBitDepth() + "      Refresh Rate = " + displays[i].getRefreshRate() + " hertz");
		}
	
		/**
		 * Prints out the Default Buffer Capabilities that can be used to figure out changes that can be made to the capabilities.
		 * These capabilities may not reflect the current set Buffer Capabilities for the active Buffer Strategy used for rendering
		 * to the GameWindow.  The "Is Full Screen Required for Page Flipping" and "Are More Than Two Buffers Supported for Page
		 * Flipping" protions should be the same between the default and current Buffer Capabilities.
		 */
		public void printDefualtBufferCapabilities() {
			GraphicsEnvironment ge = GraphicsEnvironment.getLocalGraphicsEnvironment();
			GraphicsDevice screen = ge.getDefaultScreenDevice();
			GraphicsConfiguration config = screen.getDefaultConfiguration();
			BufferCapabilities capab = config.getBufferCapabilities();
			System.out.println("--- CURRENT BUFFER CAPABILITIES ---");
			System.out.println("Are More Than Two Buffers Supported for Page Flipping: " + capab.isMultiBufferAvailable());
			System.out.println("Number of Current Buffers: " + _numBuffers);
			System.out.println("Is Full Screen Required for Page Flipping: " + capab.isFullScreenRequired());
			System.out.println("Is Page Flipping Used: " + capab.isPageFlipping());
			System.out.println("Is Blting used: " + (capab.getFlipContents() == null));
			System.out.println("Flip Contents: " + capab.getFlipContents());
		}
	
		/**
		 * Prints out the currently set Buffer Capabilities for the Buffer Strategy of the GameWindow (which is used to render).
		 */
		public void printCurrentBufferCapabilities() {
			_gameFrame.printCurrentBufferCapabilities();
		}
	
		/**
		 * Returns the Buffer Capabilities for the GameWindow.  (Can be used to determine possible ways to set the Buffer
		 * Capabilities)
		 */
		public BufferCapabilities getBufferCapabilities() {
			return _gameFrame.getBufferCapabilities();
		}
		
		/**
		 * Sets the preferred BufferCapabilities for the buffers used in rendering (with double buffering) within the GameWindow.
		 * If unsupported capabilities are specified, then the buffer capabilities present before a call to this method will be used,
		 * and this method will throw an AWTException.
		 * @param bufferSwapMeth The buffer swapping method that should be used to display back buffer data as front buffer
		 * data. It is not guaranteed that the input buffer swap method will be the one used.  The suggested method may not be
		 * available given the current settings on the current system.  The closest method to the one specified will be used.
		 * One can always probe the available buffer swap modes by getting a GameWindow reference and calling the
		 * getBufferCapabilities() function on it before calling start() on the Driver instance. (specify null for pure blt)
		 * @param numBuffers The number of buffers to use for rendering (including the front or screen buffer).
		 * @throws AWTException If the supplied bufferSwapMeth is not supported, or numBuffers is too high.
		 */
		public void setPreferredBufferAttributes(FlipContents bufferSwapMeth, int numBuffers) throws AWTException {
			BufferCapabilities bufferCapab;
			bufferCapab = new BufferCapabilities(new ImageCapabilities(true), new ImageCapabilities(true), bufferSwapMeth);
			setPreferredBufferAttributes(bufferCapab, _numBuffers);
			_bufferSwapMeth = bufferSwapMeth;
			_numBuffers = numBuffers;
		}
		
		/**
		 * Sets the preferred BufferCapabilities for the buffers used in rendering (with double buffering) within the GameWindow.
		 * If unsupported capabilities are specified, then the buffer capabilities present before a call to this method will be used,
		 * and this method will throw an AWTException.
		 * @param bufferCapabilities The buffer capabilities that we will try to create a buffer strategy with.
		 * @param numBuffers The number of buffers to use for rendering (including the front or screen buffer).
		 * @throws AWTException If the supplied bufferCapabilities are not supported, or numBuffers is too high.
		 */
		private void setPreferredBufferAttributes(BufferCapabilities bufferCapabilities, int numBuffers) throws AWTException {
			_gameFrame.setPreferredBufferAttributes(bufferCapabilities, numBuffers);
			_numBuffers = numBuffers;
		}
		
		/**
		 * Returns the number of buffers being used for double buffering within the GameWindow's BufferStrategy (includes
		 * front or screen buffer).
		 */
		public int getNumBuffers() {
			return _numBuffers;
		}
		
		/**
		 * Sets the number of buffers to be used for double buffering within the GameWindow's BufferStrategy.
		 * @param numBuffers The number of buffers to use for double buffering (includes front or screen buffer).
		 * @throws AWTException Thrown if the number of buffers is set to high.
		 */
		public void setNumBuffer(int numBuffers) throws AWTException {
			BufferCapabilities bufferCapab;
			bufferCapab = new BufferCapabilities(new ImageCapabilities(true), new ImageCapabilities(true), _bufferSwapMeth);
			setPreferredBufferAttributes(bufferCapab, _numBuffers);
			_numBuffers = numBuffers;
		}
		
		/**
		 * Returns the method used to display back buffer data as front buffer data (returns null for pure blt). 
		 */
		public FlipContents getBufferSwapMethod() {
			return _bufferSwapMeth;
		}
		
		/**
		 * Attempts to set the buffer swap method to the specified one.
		 * @param bufferSwapMeth The buffer swapping method that should be used to display back buffer data as front buffer
		 * data. It is not guaranteed that the input buffer swap method will be the one used.  The suggested method may not be
		 * available given the current settings on the current system.  The closest method to the one specified will be used.
		 * One can always probe the available buffer swap modes by getting a GameWindow reference and calling the
		 * getBufferCapabilities() function on it before calling start() on the Driver instance. (specify null for pure blt)
		 * @return The actual set buffer swap method.
		 */
		public FlipContents setBufferSwapMethod(FlipContents bufferSwapMeth) {
			if(_bufferSwapMeth != bufferSwapMeth) {
				BufferCapabilities bufferCapab = getBufferCapabilities();
				if(bufferCapab.isPageFlipping()
				&& (bufferCapab.isFullScreenRequired() || _windowMode == WindowMode.Windowed))
					_bufferSwapMeth = bufferSwapMeth;
				else
					_bufferSwapMeth = null;
				bufferCapab = new BufferCapabilities(new ImageCapabilities(true), new ImageCapabilities(true), _bufferSwapMeth);
				try {  setPreferredBufferAttributes(bufferCapab, _numBuffers);  }
				catch (AWTException e) {  e.printStackTrace();  } // Should be exception safe since we check above...
			}
			return _bufferSwapMeth;
		}
		
		/**
		 * Sets the layout manager for the GameWindow (Causes a refresh to be generated).
		 * @param layout The new layout to be set for the GameWindow.
		 */
		public void setLayout(LayoutManager layout) {
			_gameFrame.setLayout(layout);
		}
		
		/**
		 * Adds an AWT (or Swing) Component to the GameWindow.
		 * NOTE: Swing Components will flicker if the screen is customly redrawn by a GameWindowRenderer Object.
		 * In order to make Components look smooth, one should strictly adhere to using AWT Components
		 * (unless one is not performing custom drawing within the Render function of a GameWindowRenderer Object).
		 * @param component The component to be added.
		 */
		public void addComponent(Component component) {
			_gameFrame.add(component);
			_components.add(component);
		}
		
		/**
		 * Removes an AWT (or Swing) component from the GameWindow.
		 * @param component The component to be removed from the GameWindow.
		 */
		public void removeComponent(Component component) {
			_gameFrame.remove(component);
			_components.remove(component);
		}
		
		/**
		 * Adds a GameWindowComponent object to the GameWindow.  The component will be painted on top of everything
		 * else contained in the window (i.e. everything painted by the current GameState object), and will respond to its defined
		 * input events (addListenersTo() will be called).  The component is removed from the window upon a call to
		 * removeGameWindowComponent() or removeEverything().
		 * @param component The GameWindowComponent to be added to the GameWindow.
		 */
		public void addGameWindowComponent(GameWindowComponent component) {
			_gameComponents.add(component);
			component.addListenersTo(this);
		}
	
		/**
		 * Removes a GameWindowComponent object from the GameWindow.
		 * @param component The GameWindowComponent to be removed from the GameWindow.
		 */
		public void removeGameWindowComponent(GameWindowComponent component) {
			component.removeListenersFrom(this);
			_gameComponents.remove(component);
		}
	
		/**
		 * Sets the title of the GameWindow (Not observable when in FullScreen or SimulatedFullScreen Mode).
		 * @param title The new title to be set.
		 */
		public void setTitle(String title) {
			_gameFrame.setTitle(title);
		}
	
		/**
		 * Renders the entire contents of the game to the GameWindow (called from the Central Game Loop Driver).
		 * @param interpolation Interpolation value for active rendering between logical updates.
		 */
		public void render(float interpolation) {
			Area totalRefreshArea = null;
			if(width() != _prevDim.width || height() != _prevDim.height) {
				totalRefreshArea = new Area(new Rectangle(dim()));
				_prevDim = dim();
				Driver.getGameState().handleResize(dim());
			}
			_gameFrame.render(interpolation, totalRefreshArea);
		}
	
		/**
		 * Returns the size of the content area of the GameWindow (excluding the frame).
		 */
		public Dimension dim() {
			return _gameFrame.getSize();
		}
	
		/**
		 * Returns the width of the content area of the GameWindow (excluding the frame).
		 */
		public int width() {
			return _gameFrame.getSize().width;
		}
	
		/**
		 * Returns the height of the content area of the GameWindow (excluding the frame).
		 */
		public int height() {
			return _gameFrame.getSize().height;
		}
	
		/**
		 * Returns the GameWindowComponents that have been added to the GameWindow in the form of an array.
		 * NOTE: Modifying the referenced array of components may generate inconsistencies with the internal state of the
		 * GameWindow.  If one is to modify the list of components, one should either use the addGameWindowComponent(),
		 * removeGameWindowComponent(), or removeEverything() methods.
		 */
		public ArrayList<GameWindowComponent> getGameComponents() {
			return _gameComponents;
		}
		
		/**
		 * Returns the AWT (or Swing) Components that have been added to the GameWindow in the form of an array.
		 * NOTE: Modifying the referenced array of components may generate inconsistencies with the internal state of the
		 * GameWindow.  If one is to modify the list of components, one should either use the addComponent(),
		 * removeComponent(), or removeEverything() methods.
		 */
		public ArrayList<Component> getComponents() {
			return _components;
		}
	
		/**
		 * Removes all listeners and components added to the GameWindow from a GameState object.
		 */
		public void removeEverything() {
			removeEverything(true);
		}
	
		private void removeEverything(boolean destroyCompList) {
			_gameFrame.removeEverything();
			if(destroyCompList) {
				_components.clear();
				_gameComponents.clear();
				_mouseListeners.clear();
				_keyListeners.clear();
				_mouseMotionListeners.clear();
			}
		}
	
		/**
		 * Closes the GameWindow and releases its used native resources.
		 */
		public void closeWindow() {
			closeWindow(true);
		}
	
		private void closeWindow(boolean destroyListeners) {
			removeEverything(destroyListeners);
			releaseBufferStrategy();
			if(_windowMode != WindowMode.Windowed) {
				GraphicsEnvironment ge = GraphicsEnvironment.getLocalGraphicsEnvironment();
				GraphicsDevice screen = ge.getDefaultScreenDevice();
				screen.setFullScreenWindow(null);
			}
			_gameFrame.dispose();
		}
	
		private void releaseBufferStrategy() {
			_gameFrame.releaseBufferStrategy();
		}

		@Override public void windowDeiconified(WindowEvent e) {
			Driver.getGameState().setPause(false);
			_prevDim.width = 0;
		}
		
		@Override public void windowIconified(WindowEvent e) {
			Driver.getGameState().setPause(true);
		}
		
		@Override public void keyPressed(KeyEvent e) {
			switch(e.getKeyCode()) {
			case KeyEvent.VK_P:			Driver.getGameState().setPause(!Driver.getGameState().isPaused());		break;
			case KeyEvent.VK_ESCAPE:	Driver.stop();																					break;
			}
		}
	
		/**
		 * Adds a mouse listener to the GameWindow.
		 */
		public void addMouseListener(MouseListener l) {
			_mouseListeners.add(l);
		}
	
		/**
		 * Adds a key listener to the GameWindow.
		 */
		public void addKeyListener(KeyListener l) {
			_keyListeners.add(l);
			_gameFrame.addKeyListener(l);
		}
	
		/**
		 * Adds a mouse motion listener to the GameWindow.
		 */
		public void addMouseMotionListener(MouseMotionListener l) {
			_mouseMotionListeners.add(l);
		}
		
		/**
		 * Removes a mouse listener from the GameWindow.
		 */
		public void removeMouseListener(MouseListener l) {
			_mouseListeners.remove(l);
		}
	
		/**
		 * Removes a key listener from the GameWindow.
		 */
		public void removeKeyListener(KeyListener l) {
			_keyListeners.remove(l);
			_gameFrame.removeKeyListener(l);
		}
	
		/**
		 * Removes a mouse motion listener from the GameWindow.
		 */
		public void removeMouseMotionListener(MouseMotionListener l) {
			_mouseMotionListeners.remove(l);
		}

		@Override public void windowClosing(WindowEvent e) {
			Driver.stop();
		}
		
		@Override public void mouseDragged(MouseEvent e) {
			MouseEvent e2 = new MouseEvent(e.getComponent(), e.getID(), e.getWhen(), e.getModifiers(),
					e.getX() - _gameFrame.getInsets().left, e.getY() - _gameFrame.getInsets().top,
					e.getXOnScreen(), e.getYOnScreen(),
					e.getClickCount(), e.isPopupTrigger(), e.getButton());
			for(int i = 0 ; i < _mouseMotionListeners.size() ; i++)
				_mouseMotionListeners.get(i).mouseDragged(e2);
		}

		@Override public void mouseMoved(MouseEvent e) {
			MouseEvent e2 = new MouseEvent(e.getComponent(), e.getID(), e.getWhen(), e.getModifiers(),
					  e.getX() - _gameFrame.getInsets().left, e.getY() - _gameFrame.getInsets().top,
					  e.getXOnScreen(), e.getYOnScreen(),
					  e.getClickCount(), e.isPopupTrigger(), e.getButton());
			for(int i = 0 ; i < _mouseMotionListeners.size() ; i++)
				_mouseMotionListeners.get(i).mouseMoved(e2);
		}

		@Override public void mouseClicked(MouseEvent e) {
			MouseEvent e2 = new MouseEvent(e.getComponent(), e.getID(), e.getWhen(), e.getModifiers(),
					  e.getX() - _gameFrame.getInsets().left, e.getY() - _gameFrame.getInsets().top,
					  e.getXOnScreen(), e.getYOnScreen(),
					  e.getClickCount(), e.isPopupTrigger(), e.getButton());
			for(int i = 0 ; i < _mouseListeners.size() ; i++)
				_mouseListeners.get(i).mouseClicked(e2);
		}

		@Override public void mouseEntered(MouseEvent e) {
			MouseEvent e2 = new MouseEvent(e.getComponent(), e.getID(), e.getWhen(), e.getModifiers(),
					  e.getX() - _gameFrame.getInsets().left, e.getY() - _gameFrame.getInsets().top,
					  e.getXOnScreen(), e.getYOnScreen(),
					  e.getClickCount(), e.isPopupTrigger(), e.getButton());
			for(int i = 0 ; i < _mouseListeners.size() ; i++)
				_mouseListeners.get(i).mouseEntered(e2);
		}

		@Override public void mouseExited(MouseEvent e) {
			MouseEvent e2 = new MouseEvent(e.getComponent(), e.getID(), e.getWhen(), e.getModifiers(),
					  e.getX() - _gameFrame.getInsets().left, e.getY() - _gameFrame.getInsets().top,
					  e.getXOnScreen(), e.getYOnScreen(),
					  e.getClickCount(), e.isPopupTrigger(), e.getButton());
			for(int i = 0 ; i < _mouseListeners.size() ; i++)
				_mouseListeners.get(i).mouseExited(e2);
		}

		@Override public void mousePressed(MouseEvent e) {
			MouseEvent e2 = new MouseEvent(e.getComponent(), e.getID(), e.getWhen(), e.getModifiers(),
					  e.getX() - _gameFrame.getInsets().left, e.getY() - _gameFrame.getInsets().top,
					  e.getXOnScreen(), e.getYOnScreen(),
					  e.getClickCount(), e.isPopupTrigger(), e.getButton());
			for(int i = 0 ; i < _mouseListeners.size() ; i++)
				_mouseListeners.get(i).mousePressed(e2);
		}

		@Override public void mouseReleased(MouseEvent e) {
			MouseEvent e2 = new MouseEvent(e.getComponent(), e.getID(), e.getWhen(), e.getModifiers(),
					  e.getX() - _gameFrame.getInsets().left, e.getY() - _gameFrame.getInsets().top,
					  e.getXOnScreen(), e.getYOnScreen(),
					  e.getClickCount(), e.isPopupTrigger(), e.getButton());
			for(int i = 0 ; i < _mouseListeners.size() ; i++)
				_mouseListeners.get(i).mouseReleased(e2);
		}
	
		/* Unused Listeners - must be present to satisfy compiler */
		@Override public void windowActivated(WindowEvent e){}
		@Override public void windowDeactivated(WindowEvent e){}
		@Override public void windowOpened(WindowEvent e){}
		@Override public void windowClosed(WindowEvent e){}
		@Override public void keyReleased(KeyEvent e){}
		@Override public void keyTyped(KeyEvent e){}
	}
}