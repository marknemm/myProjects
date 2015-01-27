package gamegui;

import java.awt.geom.Area;

/**
 * Contains a refresh request's area and origin.
 * @author Nemmer
 */
public class RefreshRequest {

	/**
	 * The origin of a refresh request accompanying a render() call.
	 * @author Nemmer
	 */
	public enum RefreshOrigin {  GameWindow, GameWindowRenderer  };
	
	private Area _area;
	private RefreshOrigin _origin;
	
	/**
	 * Creates an empty RefreshRequest object.
	 */
	public RefreshRequest() {
		_area = new Area();
		_origin = null;
	}
	
	/**
	 * Creates a RefreshRequest object by setting its arguments (will not ever modify the passed references).
	 * @param area The area of the refresh request.
	 * @param origin The origin of the refresh request.
	 */
	public RefreshRequest(Area area, RefreshOrigin origin) {
		_area = new Area();
		_area.add(area);
		_origin = origin;
	}
	
	/**
	 * Sets the refresh area (will not ever modify the passed reference).
	 * @param area The new refresh area.
	 */
	public void setArea(Area area) {
		_area.reset();
		_area.add(area);
	}
	
	/**
	 * Sets the refresh origin.
	 * @param origin The new refresh origin.
	 */
	public void setOrigin(RefreshOrigin origin) {
		_origin = origin;
	}
	
	/**
	 * Adds an area to the already existing refresh area.
	 * @param area The area to be added or or'd to an existing area.
	 */
	public void addArea(Area area) {
		_area.add(area);
	}
	
	/**
	 * Intersects an area with the already existing refresh area.
	 * @param area The area to be intersected with.
	 */
	public void intersectArea(Area area) {
		_area.intersect(area);
	}
	
	/**
	 * Returns a reference to the refresh area.  This value should be added to a local copy of a GameWindowRenderer's
	 * refresh area so that the RefreshRequest object's area is not accidentally modified before being passed to a child
	 * GameWindowRenderer.
	 */
	public Area getArea() {
		return _area;
	}
	
	/**
	 * Returns the origin of a refresh request (GameWindow or GameWindowRenderer).
	 */
	public RefreshOrigin getOrigin() {
		return _origin;
	}
}