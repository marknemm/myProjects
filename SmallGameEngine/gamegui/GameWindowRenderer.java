package gamegui;

import java.awt.Dimension;
import java.awt.Graphics2D;
import java.awt.geom.Area;

/**
 * All objects that draw to the Game Window must implement this interface.
 * @author Nemmer
 */
public interface GameWindowRenderer {	
	/**
	 * Called whenever a render is scheduled by the central game loop.  If this GameWindowRenderer object is also a GameState object, then
	 * this method will be called by the GameWindow in coordination with the central game loop Driver.  Otherwise, this method must be called
	 * from within the current GameState object's render() method if it is to draw to the GameWindow during a GUI update scheduled by the central
	 * game loop Driver.
	 * @param g A graphics context belonging to the main Game Window.
	 * @param interpolation The interpolation amount between two successive updates (set to 0.0f if to be ignored).
	 * @param refresh A request to refresh the graphical contents of the GameWindowRenderer.  The renderer may choose to
	 * 				 accept or ignore such a request.  All requests with origin equal to the GameWindow should not be ignored, or
	 * 				 the GameWindowRenderer's graphical contents risk being fully or partially covered up and unseen.
	 * @return The area within the GUI that was updated.  If no graphical update took place, then the returned Area shall be empty.
	 * This return value is crucial for the GameWindow to know which areas were updated so that the front buffer or display can reflect
	 * the changes made on the back buffer.  WARNING: If null is returned, then a NullPointerException will likely occur.
	 */
	public Area render(Graphics2D g, float interpolation, RefreshRequest refresh);
	/**
	 * Notifies the GameWindowRenderer object when the GameWindow is resized.
	 * @param newSize The new size of the GameWindow.
	 */
	public void handleResize(Dimension newSize);
}