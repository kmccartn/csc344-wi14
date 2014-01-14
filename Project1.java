import java.io.*;

public class Project1
{
	public static void main(String[] args)
	{
		WavFile wavIn, wavOut;		// args[0], args[1]
		int sampleRate = 44100;		// Samples per second
		int duration = 30;			// duration of out in seconds
		int numFrames = sampleRate * duration;
		// holds first 6 minutes of song
		double inBuffer[][] = new double[2][360 * sampleRate];
		double outBuffer[][] = new double[2][numFrames];
	
		try
		{
			wavIn = WavFile.openWavFile(new File(args[0]));
			wavIn.readFrames(inBuffer, 360 * sampleRate);
			wavOut = WavFile.newWavFile(new File(args[1]), 2, numFrames, 16, sampleRate);

			for (int i = 0; i < numFrames; i++)
			{
				if (i % 4 == 0)
				{
					outBuffer[0][i] = inBuffer[0][2 * i];
					outBuffer[1][i] = inBuffer[1][2 * i];
				}
				else
				{
					outBuffer[0][i] = inBuffer[0][i];
					outBuffer[1][i] = inBuffer[1][i];
				}
			}
			
			wavOut.writeFrames(outBuffer, numFrames);
			wavIn.close();
			wavOut.close();
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
}
