package gamegui.buttons;

import gamegui.GameGUI.GameWindow;
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionListener;
import java.awt.geom.Area;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.util.LinkedList;

/**
 * A simple push button used within menus.
 * @author Nemmer
 */
public class MenuButton extends GameButton implements MouseMotionListener, KeyListener {
	
	private Area _updateArea;
	private int _activateKeyCode;
	private LinkedList<Integer> _events;
	private Point _prevMouseLoc;
	
	public MenuButton(Rectangle bounds, String text) {
		super(bounds, text);
		_updateArea = new Area(_bounds);
		_buttonImgs = new BufferedImage[3];
		_activateKeyCode = -1;
		_events = new LinkedList<Integer>();
		_prevMouseLoc = new Point(-1, -1);
	}
	
	public void setFont(Font font) {
		super.setFont(font);
		_updateArea.add(new Area(_bounds));
	}
	
	public void setTextColor(Color color) {
		super.setTextColor(color);
		_updateArea.add(new Area(_bounds));
	}
	
	public void setText(String text) {
		super.setText(text);
		_updateArea.add(new Area(_bounds));
	}
	
	public void setButtonColor(Color color) {
		super.setButtonColor(color);
		_updateArea.add(new Area(_bounds));
	}
	
	public void setActivateKeyCode(int activateKeyCode) {
		_activateKeyCode = activateKeyCode;
	}
	
	public void setNormalButtonImg(BufferedImage buttonImg) {
		super.setNormalButtonImg(buttonImg);
		_updateArea.add(new Area(_bounds));
	}
	
	public void setHoverButtonImg(BufferedImage buttonImg) {
		super.setHoverButtonImg(buttonImg);
		_updateArea.add(new Area(_bounds));
	}
	
	public void setPressedButtonImg(BufferedImage buttonImg) {
		super.setPressedButtonImg(buttonImg);
		_updateArea.add(new Area(_bounds));
	}
	
	private boolean isMouseOverButton(Point mouseLoc) {
		int x = mouseLoc.x;
		int y = mouseLoc.y;
		return (x >= _bounds.x && y >= _bounds.y && x <= (int)_bounds.getMaxX() && y <= (int)_bounds.getMaxY());
	}
	
	@Override public void keyPressed(KeyEvent e) {
		if(e.getKeyCode() == _activateKeyCode)
			_events.add(GameButtonEvent.PRESSED);
	}

	@Override public void keyReleased(KeyEvent e) {
		if(e.getKeyCode() == _activateKeyCode)
			_events.add(GameButtonEvent.RELEASED_IN_REGION);
	}

	@Override public void mousePressed(MouseEvent e) {
		if(isMouseOverButton(e.getPoint()))
			_events.add(GameButtonEvent.PRESSED);
	}

	@Override public void mouseReleased(MouseEvent e) {
		if(_state == State.PRESSED) {
			if(isMouseOverButton(e.getPoint()))
				_events.add(GameButtonEvent.RELEASED_IN_REGION);
			else
				_events.add(GameButtonEvent.RELEASED_OUT_REGION);
		}
	}
	
	@Override public void mouseDragged(MouseEvent e) {
		if(isMouseOverButton(e.getPoint())) {
			if(!isMouseOverButton(_prevMouseLoc))
				_events.add(GameButtonEvent.MOUSE_ENTER_REGION);
		}
		else if(isMouseOverButton(_prevMouseLoc))
			_events.add(GameButtonEvent.MOUSE_EXIT_REGION);
		_prevMouseLoc.setLocation(e.getPoint());
	}

	@Override public void mouseMoved(MouseEvent e) {
		if(isMouseOverButton(e.getPoint())) {
			if(!isMouseOverButton(_prevMouseLoc))
				_events.add(GameButtonEvent.MOUSE_ENTER_REGION);
		}
		else if(isMouseOverButton(_prevMouseLoc))
			_events.add(GameButtonEvent.MOUSE_EXIT_REGION);
		_prevMouseLoc.setLocation(e.getPoint());
	}
	
	private void changeState(State state) {
		_state = state;
		_updateArea.add(new Area(_bounds));
	}
	
	@Override public void processInput() {
		int event;
		GameButtonEvent eventToListener = new GameButtonEvent();
		
		while(_events.size() > 0) {
			event = _events.pop();
			switch(event) {
			case GameButtonEvent.MOUSE_ENTER_REGION:
				if(_state != State.PRESSED)
					changeState(State.HOVER);
				eventToListener.setEvent(GameButtonEvent.MOUSE_ENTER_REGION);
				break;
			case GameButtonEvent.MOUSE_EXIT_REGION:
				if(_state != State.PRESSED)
					changeState(State.NORMAL);
				eventToListener.setEvent(GameButtonEvent.MOUSE_EXIT_REGION);
				break;
			case GameButtonEvent.PRESSED:
				changeState(State.PRESSED);
				eventToListener.setEvent(GameButtonEvent.PRESSED);
				break;
			case GameButtonEvent.RELEASED_IN_REGION:
				changeState(State.HOVER);
				eventToListener.setEvent(GameButtonEvent.RELEASED_IN_REGION);
				break;
			case GameButtonEvent.RELEASED_OUT_REGION:
				changeState(State.NORMAL);
				eventToListener.setEvent(GameButtonEvent.RELEASED_OUT_REGION);
				break;
			}
			_listener.handleEvent(eventToListener, this);
		}
	}
	
	private void drawNormal(Graphics2D g) {
		g.drawImage(_buttonImgs[State.NORMAL.getStateInt()], _bounds.x, _bounds.y, _bounds.width, _bounds.height, null);
	}
	
	private void drawHover(Graphics2D g) {
		if(_buttonImgs[State.HOVER.getStateInt()] != null)
			g.drawImage(_buttonImgs[State.HOVER.getStateInt()], _bounds.x, _bounds.y, _bounds.width, _bounds.height, null);
		else
			drawNormal(g);
	}
	
	private void drawPressed(Graphics2D g) {
		if(_buttonImgs[State.PRESSED.getStateInt()] != null)
			g.drawImage(_buttonImgs[State.PRESSED.getStateInt()], _bounds.x, _bounds.y, _bounds.width, _bounds.height, null);
		else
			drawNormal(g);
	}
	
	@Override public Area render(Graphics2D g, Area refreshArea) {
		Area updatedArea = new Area();
		if(refreshArea != null)
			_updateArea.add(refreshArea);
		
		if(!_updateArea.isEmpty()) {
			updatedArea.add(_updateArea);
			g.setClip(_updateArea);
			switch(_state) {
			case NORMAL:	drawNormal(g);	break;
			case HOVER:		drawHover(g);	break;
			case PRESSED:	drawPressed(g);	break;
			}
			g.setFont(_font);
			g.setColor(_textColor);
	        Rectangle2D r = g.getFontMetrics().getStringBounds(_text, g);
	        g.drawString(_text, _bounds.x + (_bounds.width / 2) - (int)(r.getWidth() / 2),
							   _bounds.y + (_bounds.height / 2) + (int)(r.getHeight() / 3));
	        _updateArea.reset();
		}
		return updatedArea;
	}

	@Override public void addListenersTo(GameWindow parent) {
		parent.addKeyListener(this);
		parent.addMouseListener(this);
		parent.addMouseMotionListener(this);
	}
	
	@Override public void removeListenersFrom(GameWindow parent) {
		parent.removeKeyListener(this);
		parent.removeMouseListener(this);
		parent.removeMouseMotionListener(this);
	}
	
	@Override public void keyTyped(KeyEvent e){}
	@Override public void mouseClicked(MouseEvent e){}
	@Override public void mouseEntered(MouseEvent e){}
	@Override public void mouseExited(MouseEvent e){}
}