package specialcomponents;

import gamegui.GameWindowRenderer;
import gamegui.RefreshRequest;
import globalutil.ImageOptimizer;
import java.awt.Dimension;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.geom.Area;
import java.awt.image.BufferedImage;
import specialcomponents.doorimgs.DoorImgs;

public class Doors implements GameWindowRenderer {
	
	public enum DoorMove {  Open, Close  }

	private static BufferedImage _leftDoor;
	private static BufferedImage _rightDoor;
	private static int _distance, _maxDistance;
	private static Dimension _size;
	private Area _updateArea, _exposedArea;
	private boolean _lastDrawDone; // Allows first call to isDone() to return false when logic is finished to force a final render
	private DoorMove _state;
	private static final int UPDT_AMT = 8;
	private int _prevInterpAmt;
	private boolean _updated;
	
	private Doors(DoorMove state) {
		_updateArea = new Area(new Rectangle(0, 0, _size.width, _size.height));
		_exposedArea = new Area();
		_lastDrawDone = false;
		_state = state;
		_prevInterpAmt = 0;
	}
	
	private Doors(Dimension size, DoorMove state) {
		_leftDoor = ImageOptimizer.createImage(DoorImgs.class, "DoorLeft.png");
		_rightDoor = ImageOptimizer.createImage(DoorImgs.class, "DoorRight.png");
		_maxDistance = size.width / 2;
		_distance = 0;
		_size = size;
		_updateArea = new Area(new Rectangle(0, 0, _size.width, _size.height));
		_exposedArea = new Area();
		_lastDrawDone = false;
		_state = state;
		_prevInterpAmt = 0;
	}
	
	public static Doors generateDoors(Dimension size, DoorMove state) {
		if(_leftDoor == null)
			return new Doors(size, state);
		else
			return new Doors(state);
	}
	
	public void updateLogic() {
		_updateArea.add(new Area(new Rectangle(0, 0, _size.width / 2 - _distance, _size.height)));
		_updateArea.add(new Area(new Rectangle(_size.width / 2 + _distance, 0, _size.width / 2 - _distance, _size.height)));
		switch(_state) {
		case Close:
			if(_distance >= UPDT_AMT)
				_distance -= UPDT_AMT;
			else if(_distance > 0)
				_distance -= 1;
		break;
		case Open:
			if(_distance < _maxDistance - (UPDT_AMT - 1))
				_distance += UPDT_AMT;
			else if(_distance < _maxDistance)
				_distance += 1;
		}
		_exposedArea.reset();
		_exposedArea.add(_updateArea);
		_exposedArea.exclusiveOr(new Area(new Rectangle(0, 0, _size.width / 2 - _distance, _size.height)));
		_exposedArea.exclusiveOr(new Area(
				new Rectangle(_size.width / 2 + _distance, 0, _size.width / 2 - _distance, _size.height)));
		_updateArea.add(_exposedArea);
		_updated = true;
	}
	
	public Area getExposedArea() {
		return _exposedArea;
	}
	
	private int getInterpAmt(float interpolation) {
		float interpAmt = 0.0f;
		
		switch(_state) {
		case Open:
			if(_distance > _maxDistance - UPDT_AMT) {
				if(_distance == _maxDistance)
					interpAmt = 0;
				else
					interpAmt = -1;
			}
			else
				interpAmt = -UPDT_AMT;
			break;
		case Close:
			if(_distance < UPDT_AMT) {
				if(_distance == 0)
					interpAmt = 0;
				else
					interpAmt = 1;
			}
			else
				interpAmt = UPDT_AMT;
			break;
		}
		
		interpAmt *= interpolation;
		if(!_updated)
			_exposedArea.reset();
		if((int)interpAmt != _prevInterpAmt) {
			_updateArea.add(new Area(new Rectangle(0, 0, _size.width / 2 - _distance, _size.height)));
			_updateArea.add(new Area(new Rectangle(_size.width / 2 + _distance, 0, _size.width / 2 - _distance, _size.height)));
			_exposedArea.add(_updateArea);
			_exposedArea.exclusiveOr(new Area(new Rectangle(0, 0, _size.width / 2 - _distance + (int)interpAmt, _size.height)));
			_exposedArea.exclusiveOr(new Area(new Rectangle(_size.width / 2 + _distance - (int)interpAmt, 0,
									_size.width / 2 - _distance, _size.height)));
			_updateArea.add(_exposedArea);
			_prevInterpAmt = (int)interpAmt;
		}
		_updated = false;
		
		return (int)interpAmt;
	}
	
	@Override public Area render(Graphics2D g, float interpolation, RefreshRequest refresh) {
		int interpAmt = 0;
		Area updatedArea = new Area();
		if(refresh != null)
			_updateArea.add(refresh.getArea());
		
		if(interpolation != 0.0f)
			interpAmt = getInterpAmt(interpolation);
		
		if(!_updateArea.isEmpty()) {
			updatedArea.add(_updateArea);
			g.setClip(new Rectangle(_size));
			g.drawImage(_leftDoor, -_distance + interpAmt, 0, _size.width / 2, _size.height, null);
			g.drawImage(_rightDoor, (_size.width / 2) + _distance - interpAmt, 0, _size.width / 2, _size.height, null);
			_updateArea.reset();
		}
		
		return updatedArea;
	}
	
	public boolean isDone() {
		if(_state == DoorMove.Open &&  _distance == _maxDistance)
			return true;
		if(_state == DoorMove.Close && _distance == 0) {
			if(_lastDrawDone)
				return true;
			_lastDrawDone = true;
		}
		return false;
	}

	@Override public void handleResize(Dimension newSize) {
		
	}
}