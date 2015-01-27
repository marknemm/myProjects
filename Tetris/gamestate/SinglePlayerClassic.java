package gamestate;

import gamegui.RefreshRequest;
import gamegui.RefreshRequest.RefreshOrigin;
import gamestate.Transition.TransitionTo;
import java.awt.Dimension;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.geom.Area;
import driver.Driver;
import main.Main;
import tetriscomponents.ActiveBlockHandler;
import tetriscomponents.BlockScreen;
import tetriscomponents.LevelDisplay;
import tetriscomponents.NextBlockScreen;
import tetriscomponents.TetrisBlockGenerator;

public class SinglePlayerClassic extends GameState implements KeyListener {
	
	private BlockScreen _blockScreen;
	private TetrisBlockGenerator _blockGenerator;
	private ActiveBlockHandler _blockHandler;
	private NextBlockScreen _nextBlockScreen[];
	private LevelDisplay _levelDisplay;
	
	public void init() {
		super.init();
		_window.setTitle("Tetris - Single Player Classic");
		_window.addKeyListener(this);
		_blockGenerator = new TetrisBlockGenerator();
		_blockScreen = new BlockScreen(_window.dim(), _blockGenerator);
		_nextBlockScreen = new NextBlockScreen[2];
		_nextBlockScreen[0] = new NextBlockScreen(_window.dim(), _blockGenerator, 0);
		_nextBlockScreen[1] = new NextBlockScreen(_window.dim(), _blockGenerator, 1);
		_levelDisplay = new LevelDisplay(new Point(10, 30));
		_blockHandler = new ActiveBlockHandler(_blockGenerator, _blockScreen, _nextBlockScreen, _levelDisplay);
	}
	
	@Override public void processInput() {
		if(!_pause)
			_blockHandler.processInput();
	}

	@Override public void updateLogic() {
		if(!_pause) {
			if(!_blockHandler.gameOver())
				_blockHandler.updateLogic();
			else {
				Transition transition = new Transition();
				transition.init(TransitionTo.Intermission);
				Driver.setGameState(transition);
			}
		}
		else
			try {  Thread.sleep(5);  }
			catch (InterruptedException e) {  e.printStackTrace();  }
	}
	
	@Override public Area render(Graphics2D g, float interpolation, RefreshRequest refresh) {
		Area updatedArea = new Area();
		if((!_pause && !_blockHandler.gameOver()) || refresh != null) {
			if(refresh != null && refresh.getOrigin() == RefreshOrigin.GameWindow)
				g.drawImage(Main.getBackgroundImg(), 0, 0, null);
			updatedArea.add(_blockScreen.render(g, interpolation, refresh));
			updatedArea.add(_blockHandler.render(g, interpolation, refresh));
			updatedArea.add(_levelDisplay.render(g, interpolation, refresh));
			for(int i = 0 ; i < _nextBlockScreen.length ; i++)
				updatedArea.add(_nextBlockScreen[i].render(g, interpolation, refresh));
		}
		return updatedArea;
	}

	@Override public void keyPressed(KeyEvent e) {
		if(!_pause) {
			switch(e.getKeyCode()) {
			case KeyEvent.VK_W:			_blockHandler.rotateBlock();			break;
			case KeyEvent.VK_A:			_blockHandler.moveBlockLeft();	break;
			case KeyEvent.VK_S:			_blockHandler.speedMove(true);	break;
			case KeyEvent.VK_D:			_blockHandler.moveBlockRight();	break;
			case KeyEvent.VK_SPACE:	_blockHandler.dropBlock();			break;
			}
		}
	}

	@Override public void keyReleased(KeyEvent e) {
		if(e.getKeyCode() == KeyEvent.VK_S)
			_blockHandler.speedMove(false);
	}

	@Override public void keyTyped(KeyEvent e){}
	@Override public void handleResize(Dimension newSize){}
}