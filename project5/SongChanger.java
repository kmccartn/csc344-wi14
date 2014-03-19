import java.io.*;
import java.util.*;

public class SongChanger
{
   public static final int BLOCK_SIZE = 500;
   
   public static void main(String[] args)
   {
      try
      {
         double skip = 70.0;
         double duration = 60.0; // Duration of output file [seconds]
         double quartermeasure = 0.428547;
         double halfmeasure = 0.42857 * 2;     
         double measure = 0.42857 * 4;
         
         SongChangerFrame frame = new SongChangerFrame();
         
         File file = frame.getSelectedFile();
         
         if (file != null)
         {
            // Open the wav file specified as the first argument
            WavFile wavFileIn = WavFile.openWavFile(frame.getSelectedFile());       
            
            // Read sample rate [samples/second] of input file so we can use this value for output file
            int sampleRate = (int)(wavFileIn.getSampleRate());
            
            long numFramesIn = wavFileIn.getNumFrames();
            
            // Calculate the number of frames required for specified duration for easy reference
            int numFramesSkip = (int)(skip*sampleRate * 2);
            long numFramesOut = (long)(duration * sampleRate);
            long numFramesMeasure = (long)(measure * sampleRate);
            long numFramesHalfMeasure = (long)(halfmeasure * sampleRate);
            long numFramesQuarterMeasure = (long)(quartermeasure * sampleRate);
            
            // Get the number of audio channels in the wav file so that we may allocate buffer space accordingly
            int numChannels = wavFileIn.getNumChannels();

            // Prepare an output file with the length and sample rate determined prior
            WavFile wavFileOutFast = WavFile.newWavFile(new File("fast_" + file.getName()), numChannels, numFramesOut, 16, sampleRate);
            WavFile wavFileOutSlow = WavFile.newWavFile(new File("slow_" + file.getName()), numChannels, numFramesOut, 16, sampleRate);

            // Create input buffer covering whole song for easy access.
            double[] bufferIn = new double[(int)(numFramesIn * numChannels)];
            
            // Create output buffer for new song
            double[] bufferOutFast = new double[(int)(numFramesOut * numChannels)];
            double[] bufferOutSlow = new double[(int)(numFramesOut * numChannels)];
            
            // Read frames from input file to bufferIn
            wavFileIn.readFrames(bufferIn, (int)numFramesIn);
         
            ArrayList<double[]> fastBuffer = new ArrayList<double[]>();

            // Creates fast copy
            // i is bufferIn index, j is fastBuffer's index
            for (int i = 0, j = -1; i < bufferIn.length && i < bufferOutFast.length; i++) 
            {    
               if (i % BLOCK_SIZE == 0)
               {
                  fastBuffer.add(new double[BLOCK_SIZE]);
                  j++;
               }          
               fastBuffer.get(j)[i % BLOCK_SIZE] = bufferIn[i] * Math.sin(i * Math.PI / BLOCK_SIZE);  
            }

            // initialize buffer to 0
            for (int i = 0; i < bufferOutFast.length; i++)
            {
               bufferOutFast[i] = 0;
            }

            int offset = 0;
            
            // i is index for bufferOutFast, j is index for fastBuffer, and k is index for each block
            for (int i = 0, j = 0, k = 0; i < bufferOutFast.length; i++, k++)
            {
               if (i > 0 && i % BLOCK_SIZE == 0)
               {
                  j++;
                  offset += (int)(BLOCK_SIZE / 2.0);
                  k = 0;
                  
                  if (j >= fastBuffer.size())
                  {
                     break;
                  }
               }
               bufferOutFast[i - offset] += fastBuffer.get(j)[k];
            }
          
            ArrayList<double[]> slowedBuffer = new ArrayList<double[]>();
           
            // Creates slow copy
            // i is slowedBuffer index, j is bufferIn's index, and k  is double[] inside slowedBuffer's index
            for (int i = -1, j = (int)(BLOCK_SIZE / 2.0), k = 0; j < bufferIn.length; j++, k++) 
            {
               if (k % BLOCK_SIZE == 0)
               {
                  slowedBuffer.add(new double[BLOCK_SIZE]);
                  j -= (int)(BLOCK_SIZE / 2.0);
                  k = 0;
                  i++;
               }
            
               slowedBuffer.get(i)[k] = bufferIn[j] * Math.sin(k * Math.PI / BLOCK_SIZE);            
            }

            for (int i = 0; i < bufferOutSlow.length; i++)
            {
               bufferOutSlow[i] = slowedBuffer.get((int)(1.0 * i / BLOCK_SIZE))[i % BLOCK_SIZE];
            }
         
            // Write frames in bufferOut to our output file
            wavFileOutFast.writeFrames(bufferOutFast, (int)numFramesOut);
            wavFileOutSlow.writeFrames(bufferOutSlow, (int)numFramesOut);
            
            // Close the wav files
            wavFileIn.close();
            wavFileOutFast.close();
            wavFileOutSlow.close();
         }
         
         frame.dispose();
      }
      catch (Exception e)
      {
         e.printStackTrace(System.out);
      }
   }    
}
