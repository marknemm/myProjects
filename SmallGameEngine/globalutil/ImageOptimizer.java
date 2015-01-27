package globalutil;

import java.awt.Graphics2D;
import java.awt.GraphicsConfiguration;
import java.awt.GraphicsEnvironment;
import java.awt.image.BufferedImage;
import java.io.IOException;
import javax.imageio.ImageIO;

/**
 * Statically creates and/or optimizes Java Images.
 * @author Nemmer
 */
public class ImageOptimizer {
	
	/* http://stackoverflow.com/questions/196890/java2d-performance-issues */
	/**
	 * Creates an optimized buffered image from a base class in the folder with the Image and a path relative from the base class.
	 * @param baseClass Base class in the folder with the image to be created.
	 * @param path The relative path (from the base class) of the image.
	 * @return The optimized image.
	 */
	public static BufferedImage createImage(Class<?> baseClass, String path) {
		BufferedImage image = null;
//		Generate (possibly unoptimized) Buffered Image from arguments
		try {  image = ImageIO.read(baseClass.getResourceAsStream(path));  }
		catch(IOException e) {  e.printStackTrace();  }
		
//		obtain the current system graphical settings
		GraphicsConfiguration gfx_config = GraphicsEnvironment.
			getLocalGraphicsEnvironment().getDefaultScreenDevice().
			getDefaultConfiguration();

//		if image is already compatible and optimized for current system settings, simply return it
		if(image.getColorModel().equals(gfx_config.getColorModel()))
			return image;

		BufferedImage new_image = gfx_config.createCompatibleImage(
				image.getWidth(), image.getHeight(), image.getTransparency());

		Graphics2D g2d = (Graphics2D)new_image.getGraphics();
		g2d.drawImage(image, 0, 0, null);
		g2d.dispose();

		return new_image; 
	}
}