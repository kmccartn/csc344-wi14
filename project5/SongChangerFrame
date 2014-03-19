import java.io.*;
import javax.swing.*;
import javax.swing.filechooser.*;

// The GUI for SongChanger (just lets the user select song file)
public class SongChangerFrame extends JFrame
{
   private File fileSelected;

   public SongChangerFrame()
   {
      JFileChooser chooser = new JFileChooser();
      FileNameExtensionFilter filter = new FileNameExtensionFilter("WAV Files", "wav");
      chooser.setFileFilter(filter);
      
      int returnVal = chooser.showOpenDialog(this);
      
      if (returnVal == JFileChooser.APPROVE_OPTION)
      {
         fileSelected = chooser.getSelectedFile();
      }
   
      pack();
      setVisible(true);
      setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
   }
   
   public File getSelectedFile()
   {
      return fileSelected;
   }
}
