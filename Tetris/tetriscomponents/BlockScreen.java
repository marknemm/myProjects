package tetriscomponents;

import gamegui.GameWindowRenderer;
import gamegui.RefreshRequest;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.geom.Area;
import java.awt.image.BufferedImage;
import java.util.Collections;
import java.util.LinkedList;
import tetriscomponents.TetrisBlockGenerator.Block;

public class BlockScreen implements GameWindowRenderer {
	
	public enum Direction {  DownPress, DownRelease, Down, Left, Right, Rotate, Drop  }
	
	private final int NUM_ROWS, NUM_COLS;
	private final Dimension BLOCK_SIZE;
	private Rectangle _myBounds;
	private BufferedImage[][] _blocks;
	private int[] _rowCounts;
	private Area _repaintArea;
	private Rectangle[] _trajectoryRects;
	private int _numDels;
	
	public BlockScreen(Dimension screenDim, TetrisBlockGenerator blockGenerator) {
		int myWidth, myHeight;
		NUM_ROWS = 18;
		NUM_COLS = 10;
		BLOCK_SIZE = blockGenerator.getBlockSize();
		
		_blocks = new BufferedImage[NUM_ROWS][NUM_COLS];
		_rowCounts = new int[NUM_ROWS];
		myWidth = NUM_COLS * BLOCK_SIZE.width;
		myHeight = NUM_ROWS * BLOCK_SIZE.height;
		_myBounds = new Rectangle((screenDim.width / 2) - (int)((myWidth / 2) * 1.33), (screenDim.height / 2) - (myHeight / 2),
												  myWidth, myHeight);
		_repaintArea = new Area(new Rectangle(_myBounds.x - 1, _myBounds.y - 1,
																  _myBounds.width + 3, _myBounds.height + 3));
		_trajectoryRects = new Rectangle[4];
		for(int i = 0 ; i < _trajectoryRects.length ; i++)
			_trajectoryRects[i] = new Rectangle(0, 0, 0, 0);
	}
	
	public int x() {
		return _myBounds.x;
	}
	
	public int y() {
		return _myBounds.y;
	}
	
	public int width() {
		return _myBounds.width;
	}
	
	public int height() {
		return _myBounds.height;
	}
	
	public boolean isCollision(Direction direction, Point sqImgLoc) {
		int row = 0, col = 0;		
		switch(direction) {
		case Down:
			row = (sqImgLoc.y + BLOCK_SIZE.height - _myBounds.y) / 30;
			col = (sqImgLoc.x - _myBounds.x) / 30;
			break;
		case Left:
			row = (sqImgLoc.y - _myBounds.y) / 30;
			col = (sqImgLoc.x - BLOCK_SIZE.width - _myBounds.x) / 30;
			break;
		case Right:
			row = (sqImgLoc.y - _myBounds.y) / 30;
			col = (sqImgLoc.x + BLOCK_SIZE.width - _myBounds.x) / 30;
			break;
		case Rotate:
			row = (sqImgLoc.y - _myBounds.y) / 30;
			col = (sqImgLoc.x - _myBounds.x) / 30;
			break;
		default:  break;
		}
		return (row >= NUM_ROWS || col >= NUM_COLS || row < 0 || col < 0 || _blocks[row][col] != null);
	}
	
	public int getNumDels() {
		int numDels = _numDels;
		_numDels = 0;
		return numDels;
	}
	
	private void deleteRows(LinkedList<Integer> rows) {
		_numDels += rows.size();
		Collections.sort(rows);
		while(rows.size() != 0) {
			for(int r = rows.pop() ; r > 0 ; r--) {
				for(int c = 0 ; c < NUM_COLS ; c++)
					_blocks[r][c] = _blocks[r - 1][c];
				_rowCounts[r] = _rowCounts[r - 1];
			}
			_repaintArea.add(new Area(new Rectangle(_myBounds.x, _myBounds.y,
																		 _myBounds.width + 1, _myBounds.height + 1)));
		}
	}
	
	public void settleBlock(Block curBlock, Point curBlockOrg) {
		int row, col;
		LinkedList<Integer> delRows = new LinkedList<Integer>();
		Point sqImgRelOrg;
		while((sqImgRelOrg = curBlock.getNextRelSqLoc()) != null) {
			sqImgRelOrg.setLocation(curBlockOrg.x + sqImgRelOrg.x,
											   curBlockOrg.y + sqImgRelOrg.y);
			_repaintArea.add(new Area(new Rectangle(sqImgRelOrg.x, sqImgRelOrg.y, BLOCK_SIZE.width, BLOCK_SIZE.height)));
			row = (sqImgRelOrg.y - _myBounds.y) / 30;
			col = (sqImgRelOrg.x - _myBounds.x) / 30;
			_blocks[row][col] = curBlock.getSqImg();
			if(++_rowCounts[row] == NUM_COLS)
				delRows.push(row);
		}
		deleteRows(delRows);
	}
	
	private void fillScreen(Graphics2D g) {
		if(_repaintArea.getBounds().x < _myBounds.x || _repaintArea.getBounds().y < _myBounds.y)
			return;
		
		int r1, r2, c1, c2;
		c1 = (_repaintArea.getBounds().x - _myBounds.x);
		r1 = (_repaintArea.getBounds().y - _myBounds.y); 
		c2 = (c1 + _repaintArea.getBounds().width) / BLOCK_SIZE.width;
		if((r2 = (r1 + _repaintArea.getBounds().height + BLOCK_SIZE.height - 1) / BLOCK_SIZE.height) > NUM_ROWS)
			r2 = NUM_ROWS;
		c1 /= BLOCK_SIZE.width;
		r1 /= BLOCK_SIZE.height;
		for(int r = r1 ; r < r2 ; r++) {
			for(int c = c1 ; c < c2; c++) {
				if(_blocks[r][c] != null)
					g.drawImage(_blocks[r][c], _myBounds.x + (c * BLOCK_SIZE.width),
								       _myBounds.y + (r * BLOCK_SIZE.height), null);
			}
		}
	}
	
	public void repaintBackgroundAt(Rectangle bounds) {
		_repaintArea.add(new Area(bounds));
	}
	
	public void updateTrajectory(Block curBlock, Point curBlockOrg) {
		int i = 0;
		Point sqImgRelOrg, sqImgOrg = new Point();
		while((sqImgRelOrg = curBlock.getNextRelSqLoc()) != null) {
			sqImgOrg.setLocation(curBlockOrg.x + sqImgRelOrg.x,
											curBlockOrg.y + sqImgRelOrg.y);
			_repaintArea.add(new Area(_trajectoryRects[i]));
			_trajectoryRects[i] = new Rectangle(sqImgOrg.x, sqImgOrg.y, BLOCK_SIZE.width,
																_myBounds.height - sqImgOrg.y + _myBounds.y + 1);
			_repaintArea.add(new Area(_trajectoryRects[i++]));
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
			Rectangle repaintBounds = _repaintArea.getBounds();
			if(refresh != null)
				repaintBounds = repaintBounds.intersection(_myBounds);
			g.fillRect(repaintBounds.x, repaintBounds.y, repaintBounds.width, repaintBounds.height);
			g.setColor(new Color(235, 235, 255));
			for(int r = 0 ; r < _trajectoryRects.length ; r++)
				g.fillRect(_trajectoryRects[r].x, _trajectoryRects[r].y, _trajectoryRects[r].width, _trajectoryRects[r].height);
			if(_repaintArea.intersects(_myBounds)) {
				g.setColor(Color.BLACK);
				g.drawRect(_myBounds.x - 1, _myBounds.y - 1, _myBounds.width + 2, _myBounds.height + 2);
			}
			fillScreen(g);
			_repaintArea.reset();
		}
		
		return updatedArea;
	}

	@Override public void handleResize(Dimension newSize) {
		
	}
}