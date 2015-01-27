//package gamegui;
//
//import gamegui.GameGUI.GameWindow;
//import gamegui.RefreshRequest.RefreshOrigin;
//import java.awt.AWTException;
//import java.awt.BufferCapabilities;
//import java.awt.Canvas;
//import java.awt.Dimension;
//import java.awt.Graphics2D;
//import java.awt.ImageCapabilities;
//import java.awt.RenderingHints;
//import java.awt.Toolkit;
//import java.awt.event.KeyListener;
//import java.awt.event.MouseListener;
//import java.awt.event.MouseMotionListener;
//import java.awt.geom.Area;
//import java.awt.image.BufferStrategy;
//import driver.Driver;
//
//@SuppressWarnings("serial")
//public class GameCanvas extends Canvas {
//
//	private GameWindow _window;
//	private BufferStrategy _bs;
//	
//	public GameCanvas(GameWindow window) {
//		super();
//		_window = window;
//		setBackground(java.awt.Color.WHITE);
//		setIgnoreRepaint(true);
//	}
//	
//	@Override public void addNotify() {
//	    super.addNotify();
//	    BufferCapabilities bufferCapab = new BufferCapabilities(new ImageCapabilities(true),
//				 																	   new ImageCapabilities(true), _window.getBufferSwapMethod());
//	    try {  createBufferStrategy(_window.getNumBuffers(), bufferCapab);  }
//	    catch (AWTException e) {  e.printStackTrace();  }
//    	_bs = getBufferStrategy();
//    	requestFocus();
//	}
//	
//	public void printCurrentBufferCapabilities() {
//		BufferCapabilities capab = _bs.getCapabilities();
//		System.out.println("--- CURRENT BUFFER CAPABILITIES ---");
//		System.out.println("Are More Than Two Buffers Supported for Page Flipping: " + capab.isMultiBufferAvailable());
//		System.out.println("Number of Current Buffers: " + _window.getNumBuffers());
//		System.out.println("Is Full Screen Required for Page Flipping: " + capab.isFullScreenRequired());
//		System.out.println("Is Page Flipping Used: " + capab.isPageFlipping());
//		System.out.println("Is Blting used: " + (capab.getFlipContents() == null));
//		System.out.println("Flip Contents: " + capab.getFlipContents());
//	}
//	
//	public void setPreferredBufferAttributes(BufferCapabilities bufferCapabilities, int numBuffers) throws AWTException {
//		if(_bs != null)
//			_bs.dispose();
//		createBufferStrategy(numBuffers, bufferCapabilities);
//		_bs = getBufferStrategy();
//	}
//	
//	public BufferCapabilities getBufferCapabilities() {
//		return _bs.getCapabilities();
//	}
//	
//	public void render(float interpolation, Area totalRefreshArea) {
//		Area updatedArea = null;
//		RefreshRequest refreshRequest= null;
//		Graphics2D g = null;
//		
//		do {
//			do {
//				try {
//					g = (Graphics2D)_bs.getDrawGraphics();
//					g.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
//					if(totalRefreshArea != null) {
//						refreshRequest = new RefreshRequest(totalRefreshArea, RefreshOrigin.GameWindow);
//						g.setColor(getBackground());
//						g.fillRect(0, 0, width(), height());
//					}
//					updatedArea = Driver.getGameState().render(g, interpolation, refreshRequest);
//					if(totalRefreshArea != null)
//						updatedArea.add(totalRefreshArea);
//					for(int i = 0 ; i < _window.getComponents().size() ; i++)
//						updatedArea.add(_window.getComponents().get(i).render(g, updatedArea));
//				}
//				finally {  g.dispose();  }
//			} while(_bs.contentsRestored());
//			if(!updatedArea.isEmpty()) {
//				_bs.show();
//				Toolkit.getDefaultToolkit().sync();
//			}
//		} while(_bs.contentsLost());
//	}
//	
//	/**
//	 * Returns the size of the GameWindow (excluding the frame).
//	 */
//	public Dimension dim() {
//		return getSize();
//	}
//	
//	/**
//	 * Returns the width of the GameWindow (excluding the frame).
//	 */
//	public int width() {
//		return getSize().width;
//	}
//	
//	/**
//	 * Returns the height of the GameWindow (excluding the frame).
//	 */
//	public int height() {
//		return getSize().height;
//	}
//	
//	public void releaseBufferStrategy() {
//		_bs.dispose();
//	}
//	
//	/**
//	 * Removes all listeners and components added to the GameWindow from a GameState object.
//	 */
//	public void removeEverything() {
//		MouseMotionListener[] mouseMotionListeners = getMouseMotionListeners();
//		MouseListener[] mouseListeners = getMouseListeners();
//		KeyListener[] keyListeners = getKeyListeners();
//		for(int i = 0 ; i < mouseListeners.length ; i++)
//			removeMouseListener(mouseListeners[i]);
//		for(int i = 0 ; i < mouseMotionListeners.length ; i++)
//			removeMouseMotionListener(mouseMotionListeners[i]);
//		for(int i = 0 ; i < keyListeners.length ; i++)
//			if(keyListeners[i] != _window)
//				removeKeyListener(keyListeners[i]);
//	}
//}