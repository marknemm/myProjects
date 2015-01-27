package gamegui;

import gamegui.GameGUI.GameWindow;
import java.awt.Graphics2D;
import java.awt.geom.Area;

/**
 * All custom made components of the GameWindow must implement this interface
 * (i.e. Buttons, Combo Boxes, Text Boxes, etc...).
 * @author Nemmer
 */
public interface GameWindowComponent {
	/**
	 * Called by the GameWindow to render the component.
	 * @param g The Graphics context of the GameWindow.
	 * @param refreshArea Called when the component must refresh the specified area due to it possibly being overlapped.
	 * @return The area within the GUI that was updated.  If no graphical update took place, then the returned Area shall be empty.
	 * This return value is crucial for the GameWindow to know which areas were updated so that the front buffer or display can reflect
	 * the changes made on the back buffer.  WARNING: If null is returned, then a NullPointerException will likely occur.
	 */
	public Area render(Graphics2D g, Area refreshArea);
	/**
	 * Adds common Java listeners to the GameWindow for the GameWindowComponent to handle its own defined input events.
	 * This method will be called by the GameWindow when adding a GameWindowComponent to it.
	 * @param parent The GameWindow to add the listeners to.
	 */
	public void addListenersTo(GameWindow parent);
	/**
	 * Removes common Java listeners from the GameWindow that were previously added for this GameWindowComponent.
	 * This method will be called by the GameWindow when removing a GameWindowComponent from it.
	 * @param parent The GameWindow object to remove the listeners from.
	 */
	public void removeListenersFrom(GameWindow parent);
}