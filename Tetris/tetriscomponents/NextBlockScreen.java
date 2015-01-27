package tetriscomponents;

import gamegui.GameWindowRenderer;
import gamegui.RefreshRequest;
import gamegui.RefreshRequest.RefreshOrigin;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.geom.Area;
import java.awt.image.BufferedImage;
import tetriscomponents.TetrisBlockGenerator.Block;

public class NextBlockScreen implements GameWindowRenderer {

	private TetrisBlockGenerator _blockGenerator;
	private Area _repaintArea;
	private Rectangle _myBounds;
	private int _slot;
	
	public NextBlockScreen(Dimension winSize, TetrisBlockGenerator blockGenerator, int slot) {
		_slot = slot;
		_blockGenerator = blockGenerator;
		Dimension blockSize = blockGenerator.getBlockSize();
		if(slot == 0)
			_myBounds = new Rectangle(winSize.width - 180, 70, (blockSize.width * 4) + 20, (blockSize.height * 4) + 20);
		else
			_myBounds = new Rectangle(winSize.width - 180, (blockSize.height * 4) + 120, (blockSize.width * 4) + 20,
													  (blockSize.height * 4) + 20);
		_repaintArea = new Area(_myBounds);
	}
	
	public void updateNextBlock() {
		_repaintArea.add(new Area(_myBounds));
	}
	
	private void drawNextBlock(Graphics2D g) {
		Block nextBlock = null;
		if(_slot == 0)
			nextBlock = _blockGenerator.peekNextBlock();
		else
			nextBlock = _blockGenerator.peekFutureBlock();
		Point sqImgRelLoc, sqImgLoc;
		sqImgLoc = new Point();
		BufferedImage sqImg = nextBlock.getSqImg();
		Rectangle blockRect = nextBlock.getBoundingRect();
		int x = _myBounds.x + (_myBounds.width / 2) - (blockRect.width / 2);
		int y = _myBounds.y + (_myBounds.height / 2) - (blockRect.height / 2);
		while((sqImgRelLoc = nextBlock.getNextRelSqLoc()) != null) {
			sqImgLoc.setLocation(x + sqImgRelLoc.x, y + sqImgRelLoc.y);
			g.drawImage(sqImg, sqImgLoc.x, sqImgLoc.y, null);
		}
	}
	
	@Override public Area render(Graphics2D g, float interpolation, RefreshRequest refresh) {
		Area updatedArea = new Area();
		if(refresh != null) {
			_repaintArea.reset();
			_repaintArea.add(refresh.getArea());
		}
			
		if(!_repaintArea.isEmpty()) {
			updatedArea.add(_repaintArea);
			g.setClip(_repaintArea);
			g.setColor(Color.WHITE);
			g.fillRect(_myBounds.x, _myBounds.y, _myBounds.width, _myBounds.height);
			g.setColor(Color.BLACK);
			g.drawRect(_myBounds.x, _myBounds.y, _myBounds.width - 1, _myBounds.height - 1);
			if(refresh == null || refresh.getOrigin() == RefreshOrigin.GameWindow)
				drawNextBlock(g);
			_repaintArea.reset();
		}
		return updatedArea;
	}

	@Override public void handleResize(Dimension newSize) {
		
	}	
}