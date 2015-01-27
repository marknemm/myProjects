package main;

import java.awt.BufferCapabilities.FlipContents;
import java.awt.Dimension;
import java.awt.Rectangle;
import java.awt.Toolkit;
import java.awt.image.BufferedImage;
import main.backgroundimg.BackgroundImg;
import driver.Driver;
import gamestate.Menu;
import globalutil.ImageOptimizer;

public class Main {
	
	private static BufferedImage _backgroundImg;
	
	public static void main(String[] args) {
		Menu menu = new Menu();
		int width = 650, height = 650;
		Dimension screenDim = Toolkit.getDefaultToolkit().getScreenSize();
		Driver driver = new Driver(menu,
				new Rectangle((screenDim.width / 2) - (width / 2), (screenDim.height / 2) - (height / 2), width, height),
				true, FlipContents.COPIED);
//		Driver driver = new Driver(menu, null, FlipContents.COPIED);
//		gamegui.GameGUI.gameWindow().setFullScreenMode();
//		gamegui.GameGUI.gameWindow().setWindowedMode(new Rectangle(0, 0, width, height), false);
//		try {  gamegui.GameGUI.gameWindow().setPreferredBufferAttributes(FlipContents.COPIED, 2);  }
//		catch (AWTException e) {  e.printStackTrace();  }
		driver.start();
	}

	/**
	 * Returns global instance of the Tetris game wood background image.
	 */
	public static BufferedImage getBackgroundImg() {
		if(_backgroundImg != null)
			return _backgroundImg;
		return (_backgroundImg = ImageOptimizer.createImage(BackgroundImg.class, "Background.png"));
	}
}