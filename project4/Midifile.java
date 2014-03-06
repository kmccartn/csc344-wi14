import java.io.*;
import java.util.*;
import javax.sound.midi.*; // package for all midi classes

public class Midifile
{
   public static int randInt(int min, int max)
   {
      Random rand = new Random();

      int randomNum = rand.nextInt((max - min) + 1) + min;

      return randomNum;
   }

   public static void main(String args[])
   {
   	try
	{
		// Create a new MIDI sequence with 24 ticks per beat
		Sequence s = new Sequence(javax.sound.midi.Sequence.PPQ,24);

         	// Obtain a MIDI track from the sequence
		Track t = s.createTrack();

        	 // General MIDI sysex -- turn on General MIDI sound set
		byte[] b = {(byte)0xF0, 0x7E, 0x7F, 0x09, 0x01, (byte)0xF7};
		SysexMessage sm = new SysexMessage();
		sm.setMessage(b, 6);
		MidiEvent me = new MidiEvent(sm,(long)0);
		t.add(me);

         	// set tempo (meta event)
		MetaMessage mt = new MetaMessage();
         	byte[] bt = {0x02, (byte)0x00, 0x00};
		mt.setMessage(0x51 ,bt, 3);
		me = new MidiEvent(mt,(long)0);
	   	t.add(me);

         	// set track name (meta event)
		mt = new MetaMessage();
		String TrackName = new String("midifile track");
		mt.setMessage(0x03 ,TrackName.getBytes(), TrackName.length());
		me = new MidiEvent(mt,(long)0);
		t.add(me);

         	// set omni on
		ShortMessage mm = new ShortMessage();
		mm.setMessage(0xB0, 0x7D,0x00);
		me = new MidiEvent(mm,(long)0);
		t.add(me);

         	// set poly on
		mm = new ShortMessage();
		mm.setMessage(0xB0, 0x7F,0x00);
		me = new MidiEvent(mm,(long)0);
		t.add(me);

         	// set instrument to Piano
		mm = new ShortMessage();
		mm.setMessage(0xC0, 0x00, 0x00);
		me = new MidiEvent(mm,(long)0);
		t.add(me);
      
         	int currTick = 1;
         	int currNote = randInt(24, 84);
  
         	while (currTick < 10000)
         	{
            		NoteSelector note = new NoteSelector(currNote);
            		NoteNode node = new NoteNode(note.getNote(),(long) currTick, (long)(currTick += randInt(10, 30)));
            		node.addEvent(t);
            		currTick += randInt(3, 10);
            		currNote = note.nextNote();
         	}
         
         	currTick = 1;
         	int modTrack = 0;
         	while (currTick < 10000)
         	{
            		int note = NoteNode node = new NoteNode((modTrack % 6 != 0) ? 0x30 : 0x45, (long) currTick, (long)(currTick += 20));
        		node.addEvent(t);
        		currTick += 30;
            		modTrack++;
         	}

         	// set end of track (meta event) 19 ticks later
		mt = new MetaMessage();
        	byte[] bet = {}; // empty array
		mt.setMessage(0x2F,bet,0);
		me = new MidiEvent(mt, (long)180);
		t.add(me);

         	// write the MIDI sequence to a MIDI file
		File f = new File("midifile.mid");
		MidiSystem.write(s,1,f);
	}
	catch(Exception e)
	{
		System.out.println("Exception caught " + e.toString());
	}
   }
}
