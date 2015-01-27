package tetriscomponents;

import globalutil.ImageOptimizer;
import java.awt.Dimension;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.security.SecureRandom;
import tetriscomponents.blockimgs.BlockImgs;

public class TetrisBlockGenerator {
	
	private enum BlockT {
		L(0),
		BackwordL(1),
		T(2),
		Z(3),
		BackwordZ(4),
		Square(5),
		I(6);
		private final int _blockInt;
		private BlockT(int blockInt) {  _blockInt = blockInt;  }
		public int getBlockInt() {  return _blockInt;  }
	}

	public class Block {
		
		private BlockT _blockT;
		private BufferedImage _sqImg;
		private Point[] _sqLocs;
		private int _ind;
		private Rectangle _bound;
		private boolean _rotated;
		
		public Block(BlockT blockT, BufferedImage sqImg, Point[] sqLocs, Dimension dim) {
			_blockT = blockT;
			_sqImg = sqImg;
			_sqLocs = sqLocs;
			_ind = 0;
			_bound = new Rectangle(0, 0, dim.width, dim.height);
			_rotated = false;
		}
		
		public BlockT getBlockT() {  return _blockT;  }
		
		public BufferedImage getSqImg() {  return _sqImg;  }
		
		/**
		 * Returns null if we've gone through all elements.
		 * @return
		 */
		public Point getNextRelSqLoc() {
			if(_ind == 4) {
				_ind = 0;
				return null;
			}
			return _sqLocs[_ind++];
		}
		
		public void resetIter() {  _ind = 0;  }
		
		public Rectangle getBoundingRect() {  return _bound;  }
		
		public void rotate() {
			if(_blockT == BlockT.Square) // No rotation needed...
				return;
			else if(_rotated && (_blockT == BlockT.BackwordZ || _blockT == BlockT.Z || _blockT == BlockT.I)) {
				rotateBack();
				return;
			}
			
			int minX = 0, minY = 0, x, dx, dy, width = _bound.width;
			_bound.width = _bound.height;
			_bound.height = width;
			
			dx = _sqLocs[1].x;
			dy = _sqLocs[1].y;
			for(int i = 0 ; i < _sqLocs.length ; i++) {
				_sqLocs[i].x -= dx;
				_sqLocs[i].y -= dy;
				x = _sqLocs[i].x;
				_sqLocs[i].x = -_sqLocs[i].y;
				_sqLocs[i].y = x;
				_sqLocs[i].x += dx;
				_sqLocs[i].y += dy;
				if(i == 0 || minX > _sqLocs[i].x)
					minX = _sqLocs[i].x;
				if(i == 0 || minY > _sqLocs[i].y)
					minY = _sqLocs[i].y;
			}
			
			_bound.x = minX;
			_bound.y = minY;
			_rotated = !_rotated;
		}
		
		public void rotateBack() {
			if(_blockT == BlockT.Square) // No rotation needed...
				return;
			else if(!_rotated && (_blockT == BlockT.BackwordZ || _blockT == BlockT.Z || _blockT == BlockT.I)) {
				rotate();
				return;
			}
			
			int minX = 0, minY = 0, x, dx, dy, width = _bound.width;
			_bound.width = _bound.height;
			_bound.height = width;
			
			dx = _sqLocs[1].x;
			dy = _sqLocs[1].y;
			for(int i = 0 ; i < _sqLocs.length ; i++) {
				_sqLocs[i].x -= dx;
				_sqLocs[i].y -= dy;
				x = _sqLocs[i].x;
				_sqLocs[i].x = _sqLocs[i].y;
				_sqLocs[i].y = -x;
				_sqLocs[i].x += dx;
				_sqLocs[i].y += dy;
				if(i == 0 || minX > _sqLocs[i].x)
					minX = _sqLocs[i].x;
				if(i == 0 || minY > _sqLocs[i].y)
					minY = _sqLocs[i].y;
			}
			
			_bound.x = minX;
			_bound.y = minY;
			_rotated = !_rotated;
		}
	}
	
	private BlockT _nextBlockT, _futureBlockT;
	private int _numDuplicates;
	private final BlockT _intBlockConvert[];
	private final Dimension _blockSize;
	
	public TetrisBlockGenerator() {
		_blockSize = new Dimension(30, 30);
		_intBlockConvert = BlockT.values();
		_nextBlockT = generateNextBlockT();
		_futureBlockT = generateNextBlockT();
	}
	
	private BlockT generateNextBlockT() {
		SecureRandom randomGenerator = new SecureRandom();
		BlockT nextBlockT = _intBlockConvert[randomGenerator.nextInt(BlockT.I.getBlockInt() + 1)];
		while(nextBlockT == _nextBlockT) {  // We have not yet overwritten _nextBlockT with new value!!!
			if(++_numDuplicates < 2)
				break;
			else 
				nextBlockT = _intBlockConvert[randomGenerator.nextInt(BlockT.I.getBlockInt() + 1)];
		}
		if(_numDuplicates >= 2)
			_numDuplicates = 0;
		return nextBlockT;
	}
	
	public final Dimension getBlockSize() {  return (Dimension)_blockSize.clone();  }
	
	public Block peekNextBlock() {
		try {  return getNextBlock(true);  }
		catch (IOException e) {  e.printStackTrace();  }
		return null;
	}
	
	public Block getNextBlock() {
		try {  return getNextBlock(false);  }
		catch (IOException e) {  e.printStackTrace();  }
		return null;
	}
	
	private BufferedImage generateBlockImg(BlockT blockT, Point[] sqLocs, Dimension dim) {
		BufferedImage blockImg = null;
		switch(blockT) {
		case L:
			blockImg = ImageOptimizer.createImage(BlockImgs.class, "LTetrisSq.png");
			sqLocs[0] = new Point(0 * _blockSize.width, 0 * _blockSize.height);
			sqLocs[1] = new Point(0 * _blockSize.width, 1 * _blockSize.height);
			sqLocs[2] = new Point(0 * _blockSize.width, 2 * _blockSize.height);
			sqLocs[3] = new Point(1 * _blockSize.width, 2 * _blockSize.height);
			dim.setSize(2 * _blockSize.width, 3 * _blockSize.height);
			break;
		case BackwordL:
			blockImg = ImageOptimizer.createImage(BlockImgs.class, "BackwordLTetrisSq.png");
			sqLocs[0] = new Point(1 * _blockSize.width, 0 * _blockSize.height);
			sqLocs[1] = new Point(1 * _blockSize.width, 1 * _blockSize.height);
			sqLocs[2] = new Point(1 * _blockSize.width, 2 * _blockSize.height);
			sqLocs[3] = new Point(0 * _blockSize.width, 2 * _blockSize.height);
			dim.setSize(2 * _blockSize.width, 3 * _blockSize.height);
			break;
		case T:
			blockImg = ImageOptimizer.createImage(BlockImgs.class, "TTetrisSq.png");
			sqLocs[0] = new Point(0 * _blockSize.width, 0 * _blockSize.height);
			sqLocs[1] = new Point(1 * _blockSize.width, 0 * _blockSize.height);
			sqLocs[2] = new Point(2 * _blockSize.width, 0 * _blockSize.height);
			sqLocs[3] = new Point(1 * _blockSize.width, 1 * _blockSize.height);
			dim.setSize(3 * _blockSize.width, 2 * _blockSize.height);
			break;
		case Z:
			blockImg = ImageOptimizer.createImage(BlockImgs.class, "ZTetrisSq.png");
			sqLocs[0] = new Point(0 * _blockSize.width, 0 * _blockSize.height);
			sqLocs[1] = new Point(0 * _blockSize.width, 1 * _blockSize.height);
			sqLocs[2] = new Point(1 * _blockSize.width, 1 * _blockSize.height);
			sqLocs[3] = new Point(1 * _blockSize.width, 2 * _blockSize.height);
			dim.setSize(2 * _blockSize.width, 3 * _blockSize.height);
			break;
		case BackwordZ:
			blockImg = ImageOptimizer.createImage(BlockImgs.class, "BackwordZTetrisSq.png");
			sqLocs[0] = new Point(1 * _blockSize.width, 0 * _blockSize.height);
			sqLocs[1] = new Point(1 * _blockSize.width, 1 * _blockSize.height);
			sqLocs[2] = new Point(0 * _blockSize.width, 1 * _blockSize.height);
			sqLocs[3] = new Point(0 * _blockSize.width, 2 * _blockSize.height);
			dim.setSize(2 * _blockSize.width, 3 * _blockSize.height);
			break;
		case Square:
			blockImg = ImageOptimizer.createImage(BlockImgs.class, "SquareTetrisSq.png");
			sqLocs[0] = new Point(0 * _blockSize.width, 0 * _blockSize.height);
			sqLocs[1] = new Point(0 * _blockSize.width, 1 * _blockSize.height);
			sqLocs[2] = new Point(1 * _blockSize.width, 0 * _blockSize.height);
			sqLocs[3] = new Point(1 * _blockSize.width, 1 * _blockSize.height);
			dim.setSize(2 * _blockSize.width, 2 * _blockSize.height);
			break;
		case I:
			blockImg = ImageOptimizer.createImage(BlockImgs.class, "ITetrisSq.png");
			sqLocs[0] = new Point(0 * _blockSize.width, 0 * _blockSize.height);
			sqLocs[1] = new Point(0 * _blockSize.width, 1 * _blockSize.height);
			sqLocs[2] = new Point(0 * _blockSize.width, 2 * _blockSize.height);
			sqLocs[3] = new Point(0 * _blockSize.width, 3 * _blockSize.height);
			dim.setSize(1 * _blockSize.width, 4 * _blockSize.height);
		}
		return blockImg;
	}
	
	private Block getNextBlock(boolean peek) throws IOException {
		Block nextBlock;
		Dimension dim = new Dimension();
		BufferedImage blockImg;
		Point[] sqLocs = new Point[4];
		
		blockImg = generateBlockImg(_nextBlockT, sqLocs, dim);
		nextBlock = new Block(_nextBlockT, blockImg, sqLocs, dim);
		if(!peek) {
			_nextBlockT = _futureBlockT;
			_futureBlockT = generateNextBlockT();
		}
		return nextBlock;
	}
	
	public Block peekFutureBlock() {
		try {  return getFutureBlock();  }
		catch (IOException e) {  e.printStackTrace();  }
		return null;
	}
	
	private Block getFutureBlock() throws IOException {
		BufferedImage blockImg = null;
		Point[] sqLocs = new Point[4];
		Dimension dim = new Dimension();
		
		blockImg = generateBlockImg(_futureBlockT, sqLocs, dim);		
		return new Block(_futureBlockT, blockImg, sqLocs, dim);
	}
}