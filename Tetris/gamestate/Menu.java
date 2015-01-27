package gamestate;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.event.KeyEvent;
import java.awt.geom.Area;
import java.awt.image.BufferedImage;
import driver.Driver;
import buttonimg.ButtonImg;
import specialcomponents.Doors;
import specialcomponents.Doors.DoorMove;
import gamegui.RefreshRequest;
import gamegui.buttons.CheckButton;
import gamegui.buttons.CheckButton.Shape;
import gamegui.buttons.GameButton;
import gamegui.buttons.GameButtonEvent;
import gamegui.buttons.GameButtonListener;
import gamegui.buttons.MenuButton;
import gamestate.Transition.TransitionTo;
import globalutil.ImageOptimizer;

public class Menu extends GameState implements GameButtonListener {
	
	private Doors _doors;
	private CheckButton _handicapButton;
	private MenuButton _singlePlayerButton;
	public static boolean HANDICAP_ON = true;
	
	public void init() {
		super.init();
		_doors = Doors.generateDoors(_window.dim(), DoorMove.Close);
		_window.setTitle("Tetris - Main Menu");
		createButtons();
	}
	
	private void createButtons() {
		Dimension winSize = _window.dim();
		Rectangle bounds = new Rectangle(winSize.width / 5, winSize.height / 10, 3 * (winSize.width / 5), winSize.height / 10);
		BufferedImage buttonImg = null;
		_singlePlayerButton = new MenuButton(bounds, "Single Player (1)");
		buttonImg = ImageOptimizer.createImage(ButtonImg.class, "Wooden_Push_Button.png");
		_singlePlayerButton.setNormalButtonImg(buttonImg);
		buttonImg = ImageOptimizer.createImage(ButtonImg.class, "Wooden_Push_Button_Hover.png");
		_singlePlayerButton.setHoverButtonImg(buttonImg);
		buttonImg = ImageOptimizer.createImage(ButtonImg.class, "Wooden_Push_Button_Pressed.png");
		_singlePlayerButton.setPressedButtonImg(buttonImg);
		_singlePlayerButton.setActivateKeyCode(KeyEvent.VK_1);
		_singlePlayerButton.setGameButtonListener(this);
		_singlePlayerButton.setFont(new Font("Comic Sans MS", Font.BOLD, 20));
		_singlePlayerButton.setTextColor(new Color(255, 140, 0));
		_window.addGameWindowComponent(_singlePlayerButton);
		bounds = new Rectangle(winSize.width - 50, 26, 20, 20);
		_handicapButton = new CheckButton(bounds, "Enable Handicap Mode", Shape.Square);
		_handicapButton.setButtonColor(new Color(218, 165, 32));
		_handicapButton.setPressedButtonColor(new Color(218, 165, 32).brighter());
		_handicapButton.setFont(new Font("Arial", Font.BOLD, 16));
		_handicapButton.setChecked(HANDICAP_ON);
		_handicapButton.setGameButtonListener(this);
		_window.addGameWindowComponent(_handicapButton);
	}

	@Override public void processInput() {
		_singlePlayerButton.processInput();
		_handicapButton.processInput();
	}
	
	@Override public Area render(Graphics2D g, float interpolation, RefreshRequest refresh) {
		return _doors.render(g, 0.0f, refresh);
	}
	
	@Override public void handleEvent(GameButtonEvent e, GameButton from) {
		if(from == _singlePlayerButton) {
			if(e.getEvent() == GameButtonEvent.RELEASED_IN_REGION) {
				Transition transition = new Transition();
				transition.init(TransitionTo.Game);
				Driver.setGameState(transition);
			}
		}
		else if(from == _handicapButton)
			if(e.getEvent() == GameButtonEvent.PRESSED)
				HANDICAP_ON = !HANDICAP_ON;
	}
	
	@Override public void updateLogic() {
		Thread.yield();
	}

	@Override public void handleResize(Dimension newSize){}
}