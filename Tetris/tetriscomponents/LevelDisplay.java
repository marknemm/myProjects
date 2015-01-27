package tetriscomponents;

import gamegui.GameWindowRenderer;
import gamegui.RefreshRequest;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.geom.Area;
import main.Main;

public class LevelDisplay implements GameWindowRenderer {

	private int _iLevel;
	private String _level;
	private Point _loc;
	private Font _font;
	private Area _repaintArea;
	
	public LevelDisplay(Point loc) {
		_iLevel = 1;
		_level = "Level: 1";
		_loc = loc;
		_font = new Font("Comic Sans MS", Font.ITALIC, 25);
		_repaintArea = new Area(new Rectangle(_loc.x, _loc.y - 27, 200, 30));
	}
	
	public int getLevel() {
		return _iLevel;
	}
	
	public void setLevel(int level) {
		_iLevel = level;
		_level = ("Level: " + _iLevel);
		_repaintArea.add(new Area(new Rectangle(_loc.x, _loc.y - 27, 140, 30)));
	}
	
	@Override public Area render(Graphics2D g, float interpolation, RefreshRequest refresh) {
		Area updatedArea = new Area();
		if(refresh != null) {
			_repaintArea.reset();
			_repaintArea.add(new Area(new Rectangle(_loc.x, _loc.y - 27, 140, 30)));
			_repaintArea.intersect(refresh.getArea());
		}
		
		if(_repaintArea != null) {
			updatedArea.add(_repaintArea);
			g.setClip(_repaintArea);
			g.drawImage(Main.getBackgroundImg(), 0, 0, null);
			g.setFont(_font);
			g.setColor(Color.BLACK);
			g.drawString(_level, _loc.x, _loc.y);
			_repaintArea.reset();
		}
		return updatedArea;
	}

	@Override public void handleResize(Dimension newSize) {
		
	}
}