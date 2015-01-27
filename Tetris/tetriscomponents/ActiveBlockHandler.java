package tetriscomponents;

import gamegui.GameWindowRenderer;
import gamegui.RefreshRequest;
import gamegui.RefreshRequest.RefreshOrigin;
import gamestate.Menu;
import java.awt.Dimension;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.geom.Area;
import java.awt.image.BufferedImage;
import java.util.LinkedList;
import tetriscomponents.BlockScreen.Direction;
import tetriscomponents.TetrisBlockGenerator.Block;

public class ActiveBlockHandler implements GameWindowRenderer {
	
	private int _cycles, _cycleAmt, _handicapPCycles, _handicapPCycleAmt;
	private Point _curBlockOrg;
	private Block _curBlock;
	private Area _repaintArea;
	private LinkedList<Direction> _updatesBuf;
	private TetrisBlockGenerator _blockGenerator;
	private BlockScreen _blockScreen;
	private NextBlockScreen[] _nextBlockScreen;
	private LevelDisplay _levelDisplay;
	private int _gameOver;  // count to 2 upon game over so we can finish drawing
	private final Dimension BLOCK_SIZE;
	
	public ActiveBlockHandler(TetrisBlockGenerator blockGenerator, BlockScreen blockScreen,
										  NextBlockScreen nextBlockScreen[], LevelDisplay levelDisplay)
	{
		BLOCK_SIZE = blockGenerator.getBlockSize();
		_blockGenerator = blockGenerator;
		_blockScreen = blockScreen;
		_nextBlockScreen = nextBlockScreen;
		_levelDisplay = levelDisplay;
		_handicapPCycles = _cycles = 0;
		if(Menu.HANDICAP_ON) {
			_cycleAmt = 20;
			_handicapPCycleAmt = _cycleAmt * 5;
		}
		else {
			_handicapPCycleAmt = 0;
			_cycleAmt = 20;
		}
		_curBlockOrg = new Point();
		_repaintArea = new Area();
		_updatesBuf = new LinkedList<Direction>();
		_gameOver = 0;
	}
	
	public boolean isTimeToMove() {
		if(_handicapPCycles == _handicapPCycleAmt) {
			if(++_cycles == _cycleAmt) {
				_cycles = 0;
				return true;
			}
		}
		else
			_handicapPCycles++;
		return false;
	}
	
	public void processInput() {
		if(_curBlock == null || _updatesBuf.size() == 0)
			return;
		
		Direction direction;
		_handicapPCycles = 0;
		while(_updatesBuf.size() > 0) {
			direction = _updatesBuf.pop();
			_blockScreen.repaintBackgroundAt(createUpdtRect());
			switch(direction) {
			case Left:
				if(!isCollision(Direction.Left))
					_curBlockOrg.x -= 30;
				break;
			case Right:
				if(!isCollision(Direction.Right))
					_curBlockOrg.x += 30;
				break;
			case Rotate:
				_curBlock.rotate();
				if(isCollision(Direction.Rotate))
					_curBlock.rotateBack();
				break;
			case DownPress:
				if(!Menu.HANDICAP_ON) {
					_cycleAmt = 2;
					if(_cycles >= _cycleAmt)
						_cycles = _cycleAmt - 1;
				}
				break;
			case DownRelease:
				if(!Menu.HANDICAP_ON) {
					_cycleAmt = 20;
					if(_cycles >= _cycleAmt)
						_cycles = _cycleAmt - 1;
				}
				break;
			case Drop:
				_blockScreen.repaintBackgroundAt(createUpdtRect());
				while(!isCollision(Direction.Down))
					_curBlockOrg.setLocation(_curBlockOrg.x, _curBlockOrg.y + BLOCK_SIZE.height);
				_blockScreen.settleBlock(_curBlock, _curBlockOrg);
				_curBlock = null;
				break;
			default:  break;
			}
			if(_curBlock != null) {
				_repaintArea.add(new Area(createUpdtRect()));
				_blockScreen.updateTrajectory(_curBlock, _curBlockOrg);
			}
		}
	}
	
	public void updateLogic() {
		if(_curBlock == null) {
			_curBlock = _blockGenerator.getNextBlock();
			_curBlockOrg.setLocation(_blockScreen.x() + (_blockScreen.width() / 2) - BLOCK_SIZE.width,
			    						    _blockScreen.y());
			_repaintArea.add(new Area(createUpdtRect()));
			_blockScreen.updateTrajectory(_curBlock, _curBlockOrg);
			if(isCollision(Direction.Down))
				_gameOver++;
			else {
				for(int i = 0 ; i < _nextBlockScreen.length ; i++)
					_nextBlockScreen[i].updateNextBlock();
				_levelDisplay.setLevel(_levelDisplay.getLevel() + _blockScreen.getNumDels());
				_cycleAmt = 20 - (_levelDisplay.getLevel() / 3) > 3 ? 20 - (_levelDisplay.getLevel() / 3) : 3;
				if(_cycles >= _cycleAmt)
					_cycles = _cycleAmt - 1;
				_handicapPCycles = _handicapPCycleAmt;
			}
		}
		else if(isTimeToMove()) {
			if(!isCollision(Direction.Down)) {
				_blockScreen.repaintBackgroundAt(createUpdtRect());
				_curBlockOrg.setLocation(_curBlockOrg.x, _curBlockOrg.y + BLOCK_SIZE.height);
				_repaintArea.add(new Area(createUpdtRect()));
				_blockScreen.updateTrajectory(_curBlock, _curBlockOrg);
			}
			else {
				_blockScreen.settleBlock(_curBlock, _curBlockOrg);
				_curBlock = null;
			}
		}
	}
	
	private boolean isCollision(Direction direction) {
		Point sqImgRelLoc, sqImgLoc = new Point();;
		while((sqImgRelLoc = _curBlock.getNextRelSqLoc()) != null) {
			sqImgLoc.setLocation(_curBlockOrg.x + sqImgRelLoc.x, _curBlockOrg.y + sqImgRelLoc.y);
			if(_blockScreen.isCollision(direction, sqImgLoc)) {
				_curBlock.resetIter();
				return true;
			}
		}
		return false;
	}
	
	private Rectangle createUpdtRect() {
		Rectangle boundingRect = _curBlock.getBoundingRect();
		return new Rectangle(boundingRect.x + _curBlockOrg.x, boundingRect.y + _curBlockOrg.y, boundingRect.width,
										boundingRect.height);
	}

	@Override public Area render(Graphics2D g, float interpolation, RefreshRequest refresh) {
		Area updatedArea = new Area();
		if(refresh != null && refresh.getOrigin() == RefreshOrigin.GameWindow)
			_repaintArea.add(refresh.getArea());
		
		updatedArea.add(_repaintArea);
		if(_curBlock != null && !_repaintArea.isEmpty()) {
			g.setClip(_repaintArea);
			Point sqImgRelLoc, sqImgLoc;
			sqImgLoc = new Point();
			BufferedImage sqImg = _curBlock.getSqImg();
			while((sqImgRelLoc = _curBlock.getNextRelSqLoc()) != null) {
				sqImgLoc.setLocation(_curBlockOrg.x + sqImgRelLoc.x, _curBlockOrg.y + sqImgRelLoc.y);
				g.drawImage(sqImg, sqImgLoc.x, sqImgLoc.y, null);
			}
			_repaintArea.reset();
		}
		return updatedArea;
	}
	
	public boolean gameOver() {
		return (_gameOver == 2);
	}
	
	public void speedMove(boolean press) {
		if(press)	_updatesBuf.add(Direction.DownPress);
		else			_updatesBuf.add(Direction.DownRelease);
	}
	
	public void moveBlockLeft() {  _updatesBuf.add(Direction.Left);  }
	public void moveBlockRight() {  _updatesBuf.add(Direction.Right);  }
	public void rotateBlock() {  _updatesBuf.add(Direction.Rotate);  }
	public void dropBlock() {  _updatesBuf.add(Direction.Drop);  }

	@Override public void handleResize(Dimension newSize) {
		
	}
}