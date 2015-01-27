package gamegui;

import gamegui.GameGUI.GameWindow;
import gamegui.RefreshRequest.RefreshOrigin;
import java.awt.AWTException;
import java.awt.BufferCapabilities;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Frame;
import java.awt.Graphics2D;
import java.awt.ImageCapabilities;
import java.awt.RenderingHints;
import java.awt.Toolkit;
import java.awt.event.KeyListener;
import java.awt.geom.Area;
import java.awt.image.BufferStrategy;
import driver.Driver;

/**
 * The Frame for the central GameWindow used for the entire game.
 * @author Nemmer
 */
@SuppressWarnings("serial")
public class GameFrame extends Frame {

	private GameWindow _window;
	private BufferStrategy _bs;
	
	public GameFrame(GameWindow window) {
		super();
		_window = window;
		setIgnoreRepaint(true);
	}
	
	@Override public void addNotify() {
	    super.addNotify();
    	BufferCapabilities bufferCapab = new BufferCapabilities(new ImageCapabilities(true),
    														 						  new ImageCapabilities(true), _window.getBufferSwapMethod());
    	try {  createBufferStrategy(_window.getNumBuffers(), bufferCapab);  }
		catch (AWTException e) {  e.printStackTrace();  }
    	_bs = getBufferStrategy();
	}

	/**
	 * Returns the size of the GameWindow (excluding the frame).
	 */
	public Dimension dim() {
		return new Dimension(width(), height());
	}
	
	/**
	 * Returns the width of the GameWindow (excluding the frame).
	 */
	public int width() {
		return getSize().width - (getInsets().left + getInsets().right);
	}
	
	/**
	 * Returns the height of the GameWindow (excluding the frame).
	 */
	public int height() {
		return getSize().height - (getInsets().top + getInsets().bottom);
	}
	
	public void setPreferredBufferAttributes(BufferCapabilities bufferCapabilities, int numBuffers) throws AWTException {
		if(_bs != null)
			_bs.dispose();
		createBufferStrategy(numBuffers, bufferCapabilities);
		_bs = getBufferStrategy();
	}
	
	public void printCurrentBufferCapabilities() {
		BufferCapabilities capab = _bs.getCapabilities();
		System.out.println("--- CURRENT BUFFER CAPABILITIES ---");
		System.out.println("Are More Than Two Buffers Supported for Page Flipping: " + capab.isMultiBufferAvailable());
		System.out.println("Number of Current Buffers: " + _window.getNumBuffers());
		System.out.println("Is Full Screen Required for Page Flipping: " + capab.isFullScreenRequired());
		System.out.println("Is Page Flipping Used: " + capab.isPageFlipping());
		System.out.println("Is Blting used: " + (capab.getFlipContents() == null));
		System.out.println("Flip Contents: " + capab.getFlipContents());
	}
	
	public BufferCapabilities getBufferCapabilities() {
		return _bs.getCapabilities();
	}
	
	public void render(float interpolation, Area totalRefreshArea) {
		int i;
		Area updatedArea = null;
		RefreshRequest refreshRequest = null;
		Graphics2D g = null;
		
		do {
			do {
				try {
					g = (Graphics2D)_bs.getDrawGraphics();
					g.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
					g.translate(getInsets().left, getInsets().top);
					if(totalRefreshArea != null)
						refreshRequest = new RefreshRequest(totalRefreshArea, RefreshOrigin.GameWindow);
					updatedArea = Driver.getGameState().render(g, interpolation, refreshRequest);
					if(totalRefreshArea != null)
						updatedArea.add(totalRefreshArea);
					for(i = 0 ; i < _window.getGameComponents().size() ; i++)
						updatedArea.add(_window.getGameComponents().get(i).render(g, updatedArea));
				}
				finally {  g.dispose();  }
			} while(_bs.contentsRestored());
			if(!updatedArea.isEmpty()) { // Only update the front (screen) buffer if a change was made to back buffer!
				_bs.show();
				Toolkit.getDefaultToolkit().sync();
				for(i = 0 ; i < getComponents().length ; i++)
					if(getComponent(i).isLightweight())
						getComponent(i).repaint(0);
			}
		} while(_bs.contentsLost());
	}
	
	public void removeEverything() {
		int i;
		KeyListener[] keyListeners = getKeyListeners();
		Component[] components = getComponents();
		for(i = 0 ; i < keyListeners.length ; i++)
			if(keyListeners[i] != _window)
				removeKeyListener(keyListeners[i]);
		for(i = 0 ; i < components.length ; i++)
			remove(components[i]);
	}
	
	public void releaseBufferStrategy() {
		_bs.dispose();
	}
	
	@Override public void invalidate() {  return;  }
}