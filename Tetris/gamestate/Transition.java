package gamestate;

import gamegui.RefreshRequest;
import gamegui.RefreshRequest.RefreshOrigin;
import java.awt.Dimension;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.geom.Area;
import driver.Driver;
import main.Main;
import specialcomponents.Doors;
import specialcomponents.Doors.DoorMove;

public class Transition extends GameState {

	public enum TransitionTo {  Menu, Game, Intermission  }
	
	private TransitionTo _where;
	private Doors _doors;
	private boolean _inProgress;
	private boolean _firstForceDraw;
	private Area _updateArea;
	private GameState _predrawState; // If we are opening doors in a transition, we want to draw the game behind them
	
	public void init(TransitionTo where) {
		super.init();
		_where = where;
		_updateArea = new Area();
		switch(where) {
		case Menu:
			_doors = Doors.generateDoors(_window.dim(), DoorMove.Close);
			break;
		case Intermission:
			_doors = Doors.generateDoors(_window.dim(), DoorMove.Close);
			break;
		case Game:
			_doors = Doors.generateDoors(_window.dim(), DoorMove.Open);
			_predrawState = new SinglePlayerClassic();
			_predrawState.init();
			break;
		}
		_inProgress = true;
		_firstForceDraw = true;
		_window.removeEverything();
	}

	@Override public void updateLogic() {
		if(_firstForceDraw) { // Makes sure we redraw window with doors at start state before moving doors
			_firstForceDraw = false;
			return;
		}
		
		if(_doors.isDone()) {
			_inProgress = false;
			switch(_where) {
			case Menu:
				Menu menu = new Menu();
				menu.init();
				Driver.setGameState(menu);
				break;
			case Intermission:
				Intermission intermission = new Intermission();
				intermission.init();
				Driver.setGameState(intermission);
				break;
			case Game:
				SinglePlayerClassic singlePlayerClassic = new SinglePlayerClassic();
				singlePlayerClassic.init();
				Driver.setGameState(singlePlayerClassic);
				break;
			}
		}
		else
			_doors.updateLogic();
	}
	
	@Override public Area render(Graphics2D g, float interpolation, RefreshRequest refresh) {
		Area updatedArea = new Area();
		if(refresh != null)
			_updateArea.add(refresh.getArea());
		
		if(_inProgress) {
			updatedArea.add(_doors.render(g, interpolation, refresh));
			if(!_doors.getExposedArea().isEmpty())
				_updateArea.add(_doors.getExposedArea());
			if(!_updateArea.isEmpty()) {
				updatedArea.add(_updateArea);
				g.setClip(_updateArea);
				Rectangle repaintBounds = _updateArea.getBounds();
				if(_where == TransitionTo.Game)
					g.drawImage(Main.getBackgroundImg(), repaintBounds.x, repaintBounds.y,
									    repaintBounds.width, repaintBounds.height, null);
				if(_predrawState != null) {
					if(refresh != null && refresh.getOrigin() == RefreshOrigin.GameWindow) {
						_updateArea.add(new Area(_updateArea.getBounds()));
						_updateArea.intersect(refresh.getArea());
					}
					_predrawState.render(g, interpolation, new RefreshRequest(_updateArea, RefreshOrigin.GameWindowRenderer));
				}
				_updateArea.reset();
			}
		}
		return updatedArea;
	}
	
	@Override public void processInput(){}
	@Override public void handleResize(Dimension newSize){}
}