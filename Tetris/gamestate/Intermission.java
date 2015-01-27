package gamestate;

import gamegui.RefreshRequest;
import gamegui.RefreshRequest.RefreshOrigin;
import gamestate.Transition.TransitionTo;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.geom.Area;
import java.awt.geom.Rectangle2D;
import driver.Driver;
import specialcomponents.Doors;
import specialcomponents.Doors.DoorMove;

public class Intermission extends GameState implements KeyListener {
	
	private int _countDown;
	private int _cycleAmt;
	private int _cycles;
	private Area _updateArea;
	private Rectangle _bounds;
	private Font _font, _smallFont;
	private Doors _doors;
	private boolean _keyPressed;
	
	public Intermission(){}
	
	@Override public void init() {
		super.init();
		_keyPressed = false;
		_countDown = 10;
		_cycleAmt = (int)Driver.getUPS();
		_cycles = 0;
		_bounds = new Rectangle(200, 80);
		_updateArea = new Area(_bounds);
		_smallFont = new Font("Arial", Font.PLAIN, 30);
		_font = new Font("Matura MT Script Capitals", Font.ITALIC, 200);
		_doors = Doors.generateDoors(_window.dim(), DoorMove.Close);
		_window.addKeyListener(this);
	}

	@Override public Area render(Graphics2D g, float interpolation, RefreshRequest refresh) {
		Area updatedArea = new Area();
		if(refresh != null)
			_updateArea.add(refresh.getArea());
		
		if(!_updateArea.isEmpty()) {
			g.setFont(_font);
			g.setColor(new Color(255, 140, 0));
			String countDownStr = new String("" + _countDown);
			Rectangle2D r = g.getFontMetrics().getStringBounds(countDownStr, g);
			_bounds.setSize((int)r.getWidth(), (int)r.getHeight());
			_bounds.setLocation((_window.width() / 2) - (_bounds.width / 2),
										  (_window.height() / 2) - _bounds.height);
			_updateArea.add(new Area(_bounds));
			updatedArea.add(_updateArea);
			g.setClip(_updateArea);
			_doors.render(g, interpolation, new RefreshRequest(_updateArea, RefreshOrigin.GameWindowRenderer));
			if(_countDown != 0) {
				g.drawString(countDownStr, _bounds.x, _bounds.y + _bounds.height);
				g.setFont(_smallFont);
				g.drawString("--- Press any key to Continue ---", 25, 60);
			}
			_updateArea.reset();
		}
		return updatedArea;
	}

	@Override public void updateLogic() {
		if(++_cycles == _cycleAmt) {
			if(_countDown > 0) {
				_cycles = 0;
				_countDown--;
				_updateArea.add(new Area(_bounds));
			}
			else {
				Menu menu = new Menu();
				menu.init();
				Driver.setGameState(menu);
			}
		}
	}
	
	@Override public void keyPressed(KeyEvent e) {
		_keyPressed = true;
	}
	
	@Override public void processInput() {
		if(_keyPressed) {
			Transition transition = new Transition();
			transition.init(TransitionTo.Game);
			Driver.setGameState(transition);
		}
	}
	
	@Override public void handleResize(Dimension newSize){}
	@Override public void keyReleased(KeyEvent e){}
	@Override public void keyTyped(KeyEvent e){}
}