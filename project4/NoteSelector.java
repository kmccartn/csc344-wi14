public class NoteSelector
{
   public static final int C  = 0x00;
   public static final int Ch = 0x01;
   public static final int D  = 0x02;
   public static final int Eb = 0x03;
   public static final int E  = 0x04;
   public static final int F  = 0x05;
   public static final int Fh = 0x06;
   public static final int G  = 0x07;
   public static final int Gh = 0x08;
   public static final int A  = 0x09;
   public static final int Bb = 0x0A;
   public static final int B  = 0x0B;
   
   public static final int NUM_NOTES = 84;
   public static final int NUM_OCTAVES = 8;
   public static final int DIS_NOTES = 12;

   private int note; // hex value
   
   public NoteSelector(int note)
   {
      this.note = note;
   }
   
   public int getNote()
   {
      return note;
   }
   
   public int nextNote()
   {
      double random = Math.random();
      int plainNote = note % DIS_NOTES;
      int octave = note / NUM_OCTAVES;
      double randomOctave = Math.random();
      int newNote = -1;
      
      // decides if it wants to change octaves (+/- 1 from current)
      if (randomOctave < 0.10 && octave < NUM_OCTAVES)
      {
         octave = (octave + 1) % NUM_OCTAVES;
      }
      else if (randomOctave < 0.20 && octave > 0)
      {
         octave = (octave - 1) % NUM_OCTAVES;
      }
      
      // Markov chain
      if (plainNote == Ch)
      {
         if (random < 0.75)
         {
            newNote = Eb;
         }
         else
         {
            newNote = Ch;
         }
      }
      else if (plainNote == C)
      {
         if (random < 0.75)
         {
            newNote = C;
         }
         else
         {
            newNote = D;
         }
      }
      else if (plainNote == D)
      {
         if (random < 0.75)
         {
            newNote = Ch;
         }
         else
         {
            newNote = D;
         }
      }
      else if (plainNote == Eb)
      {
         if (random < 0.75)
         {
            newNote = Fh;
         }
         else
         {
            newNote = Eb;
         }
      }
      else if (plainNote == E)
      {
         if (random < 0.75)
         {
            newNote = Gh;
         }
         else
         {
            newNote = E;
         }
      }
      else if (plainNote == F)
      {
         if (random < 0.75)
         {
            newNote = E;
         }
         else
         {
            newNote = F;
         }
      }
      else if (plainNote == Fh)
      {
         if (random < 0.75)
         {
            newNote = Gh;
         }
         else
         {
            newNote = Fh;
         }
      }
      else if (plainNote == G)
      {
         if (random < 0.75)
         {
            newNote = Bb;
         }
         else
         {
            newNote = G;
         }
      }
      else if (plainNote == Gh)
      {
         if (random < 0.75)
         {
            newNote = F;
         }
         else
         {
            newNote = Gh;
         }
      }
      else if (plainNote == A)
      {
         if (random < 0.75)
         {
            newNote = G;
         }
         else
         {
            newNote = A;
         }
      }
      else if (plainNote == Bb)
      {
         if (random < 0.75)
         {
            newNote = B;
         }
         else
         {
            newNote = Bb;
         }
      }
      else if (plainNote == B)
      {
         if (random < 0.75)
         {
            newNote = A;
         }
         else
         {
            newNote = B;
         }
      }
      
      // so it's not in the top two octaves (or higher?)
      int value = (newNote + octave * DIS_NOTES) % NUM_NOTES;
      
      // so it's not in the bottom two octaves
      if (value < 24)
      {
         value += 24;
      }
      
      return value;
   }
}
