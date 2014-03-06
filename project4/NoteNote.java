import javax.sound.midi.*;

public class NoteNode
{
   private MidiEvent startEvent, endEvent;
   
   public NoteNode(int note, long startTick, long endTick) throws InvalidMidiDataException
   {
      ShortMessage startMessage = new ShortMessage(0x90, note, 0x60);
      ShortMessage endMessage = new ShortMessage(0x80, note, 0x40);
      
      startEvent = new MidiEvent(startMessage, startTick);
      endEvent = new MidiEvent(endMessage, endTick);
   }
   
   public void addEvent(Track t)
   {
      t.add(startEvent);
      t.add(endEvent);
   }
}
