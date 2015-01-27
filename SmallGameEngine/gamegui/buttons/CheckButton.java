package gamegui.buttons;

import gamegui.GameGUI.GameWindow;
import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.Stroke;
import java.awt.event.MouseEvent;
import java.awt.geom.Area;
import java.awt.geom.Rectangle2D;

/**
 * A simple check button that can take the form of a square or circle.
 * @author Nemmer
 */
public class CheckButton extends GameButton {

	public enum Shape {  Square, Circle  };
	
	private Shape _shape;
	private Area _updateArea;
	private boolean _renderText;
	private int _numPresses;
	
	public CheckButton(Rectangle bounds, String text, Shape shape) {
		super(bounds, text);
		_shape = shape;
		_updateArea = new Area(bounds);
		_renderText = true;
		_numPresses = 0;
	}
	
	public void setButtonColor(Color color) {
		super.setButtonColor(color);
		_updateArea.add(new Area(_bounds));
	}
	
	public void setTextColor(Color color) {
		_textColor = color;
		_updateArea.add(new Area(_bounds));
		_renderText = true;
	}
	
	public void setFont(Font font) {
		_font = font;
		_updateArea.add(new Area(_bounds));
		_renderText = true;
	}
	
	public void setChecked(boolean flag) {
		if(flag)
			_state = State.PRESSED;
		else
			_state = State.NORMAL;
		_updateArea.add(new Area(_bounds));
	}
	
	public boolean isChecked() {
		return (_state == State.PRESSED);
	}
	
	private void renderSq(Graphics2D g) {
		Stroke oldStroke;
		g.setColor(_buttonColors[State.NORMAL.getStateInt()]);
		oldStroke = g.getStroke();
		g.setStroke(new BasicStroke(4));
		g.drawRect(_bounds.x, _bounds.y, _bounds.width - 1, _bounds.height - 1);
		g.setStroke(oldStroke);
		if(_state == State.PRESSED)
			g.setColor(_buttonColors[State.PRESSED.getStateInt()]);
		else {
			if(_buttonColors[State.NORMAL.getStateInt()] != Color.WHITE)
				g.setColor(Color.WHITE);
			else
				g.setColor(Color.BLACK);
		}
		g.fillRect(_bounds.x + 2, _bounds.y + 2, _bounds.width - 4, _bounds.height - 4);
	}
	
	private void renderCirc(Graphics2D g) {
		g.setColor(_buttonColors[State.NORMAL.getStateInt()]);
		g.drawOval(_bounds.x, _bounds.y, _bounds.width - 1, _bounds.height - 1);
		if(_state == State.PRESSED)
			g.setColor(_buttonColors[State.PRESSED.getStateInt()]);
		else {
			if(_buttonColors[State.NORMAL.getStateInt()] != Color.WHITE)
				g.setColor(Color.WHITE);
			else
				g.setColor(Color.BLACK);
		}
		g.fillOval(_bounds.x + 1, _bounds.y + 1, _bounds.width - 2, _bounds.height - 2);
	}
	
	@Override public Area render(Graphics2D g, Area refreshArea) {
		Area updatedArea = new Area();
		if(refreshArea != null) {
			_updateArea.add(refreshArea);
			_renderText = true;
		}
		
		if(!_updateArea.isEmpty()) {
			updatedArea.add(_updateArea);
			g.setClip(_updateArea);
	        if(_renderText) {
	        	g.setColor(_textColor);
				g.setFont(_font);
				Rectangle2D r = g.getFontMetrics().getStringBounds(_text, g);
	        	_updateArea.add(new Area(new Rectangle(_bounds.x - (int)(r.getWidth() + 7),
	        																_bounds.y + (_bounds.height / 2) - (int)(r.getHeight() / 2),
	        																(int)r.getWidth(), (int)r.getHeight())));
	        	updatedArea.add(_updateArea);
	        	g.setClip(_updateArea);
	        	g.drawString(_text, _bounds.x - (int)(r.getWidth() + 7), _bounds.y + (_bounds.height / 2) + (int)(r.getHeight() / 3));
	        }
			switch(_shape) {
			case Square:		renderSq(g);		break;
			case Circle:		renderCirc(g);		break;
			}
			_updateArea.reset();
		}
		
		return updatedArea;
	}
	
	@Override public void processInput() {
		if(_numPresses > 0) {
			if(_numPresses % 2 != 0) {
				GameButtonEvent event = new GameButtonEvent();
				event.setEvent(GameButtonEvent.PRESSED);
				if(_state == State.PRESSED)
					_state = State.NORMAL;
				else
					_state = State.PRESSED;
				_updateArea.add(new Area(_bounds));
				for(; _numPresses > 0 ; _numPresses--)
					_listener.handleEvent(event, this);
			}
		}
	}

	@Override public void addListenersTo(GameWindow parent) {
		parent.addMouseListener(this);
	}
	
	@Override public void removeListenersFrom(GameWindow parent) {
		parent.removeMouseListener(this);
	}

	@Override public void mousePressed(MouseEvent e) {
		if(_bounds.contains(e.getPoint()))
			_numPresses++;
	}

	@Override public void mouseReleased(MouseEvent e){}
	@Override public void mouseClicked(MouseEvent e){}
	@Override public void mouseEntered(MouseEvent e){}
	@Override public void mouseExited(MouseEvent e){}
}