/*
  ==============================================================================

    This file was auto-generated by the Jucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define HARMONICS 4

AudioProcessor* JUCE_CALLTYPE createPluginFilter();

//==============================================================================
/** A synth sound for project 2 */
class P2Sound : public SynthesiserSound
{
public:
    P2Sound() {}
    
    bool appliesToNote (const int /*midiNoteNumber*/)           { return true; }
    bool appliesToChannel (const int /*midiChannel*/)           { return true; }
};

//=============================================================================
/** A Wavetable **/
 
// NB: ANGLES SPECIFIED IN ROTATIONS, NOT RADIANS.

#define WAVETABLE_SIZE 1000
// this is one larger than needed, to prevent
// having to special-case the wraparound.
#define WAVETABLE_ARRAY_SIZE (WAVETABLE_SIZE + 1)
class WaveTable 
{
public:
    double table[WAVETABLE_ARRAY_SIZE];
    
    // given a number in 0<angle<1.0, return 
    // a (linearly interpolated) number from the wavetable.
    double lookup(double angle)
	{
        // yes, we could avoid all of the multiplication if we 
        // pushed the wavetable size into the angle....
        const double scaled_angle = angle * WAVETABLE_SIZE;
        const double lower = floor(scaled_angle);
        const double fraction = scaled_angle - lower;
        return (table[(int)lower] * (1.0 - fraction)) 
            + (table[(int)lower+1] * fraction);
    }

	void squareInit()
	{
        for (int i = 0; i < WAVETABLE_SIZE; i++)
		{
            table[i] =  ((i < (WAVETABLE_SIZE/2)) ? 1.0 : -1.0);
        }
        table[WAVETABLE_SIZE] = 1.0;
    }

	void sineInit()
	{
		for (int i = 0; i < WAVETABLE_SIZE; i++)
		{
			table[i] = sin(2.0 * double_Pi * ((double)i / WAVETABLE_SIZE));
		}
		table[WAVETABLE_SIZE] = 0.0;
	}
};

//==============================================================================
/** A Voice for Project 2 */
class P2Voice  : public SynthesiserVoice
{
public:
    P2Voice()
    : playing(notPlaying),
    angleDelta (0.0),
    tailOff (0.0),
	wavetable()
	{
		wavetable.squareInit();
	}
    
    bool canPlaySound (SynthesiserSound* sound)
    {
        return dynamic_cast <P2Sound*> (sound) != 0;
    }
    
    void startNote (int midiNoteNumber, float velocity,
                    SynthesiserSound* /*sound*/, int /*currentPitchWheelPosition*/)
    {
		playing = keyHeld;
        currentAngle = 0.0;
        for (int i = 0; i < HARMONICS; i++)
		{
           levels[i] = 0.25;
        }
		level = velocity * 0.15;
        
        const double cyclesPerSecond = MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        const double cyclesPerSample = cyclesPerSecond / getSampleRate();
        
        angleDelta = cyclesPerSample;

		// reset o1
        o1_angle = 0.0;
        o1_angleDelta = o1_freq / getSampleRate();
    }
    
    void stopNote (bool allowTailOff)
    {
        if (allowTailOff)
        {
            // start a tail-off by setting this flag. The render callback will pick up on
            // this and do a fade out, calling clearCurrentNote() when it's finished.
            
            if (playing == keyHeld)
			{
                // we only need to begin a tail-off if it's not already doing so - the
                // stopNote method could be called more than once.
                playing = keyReleased;
                tailOff = 1.0;
            }
        }
        else
        {
            // we're being told to stop playing immediately, so reset everything..
            clearCurrentNote();
			playing = notPlaying;
            angleDelta = 0.0;
        }
    }
    
    void pitchWheelMoved (int /*newValue*/)
    {
        // can't be bothered implementing this for the demo!
    }
    
    void controllerMoved (int /*controllerNumber*/, int /*newValue*/)
    {
        // not interested in controllers in this case.
    }
    
    void renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
    {
		if (playing != notPlaying)
		{
            const double levelMult = level * (playing ==  keyReleased ? tailOff : 1.0);
            for (int sample = startSample; sample < startSample + numSamples; sample++){
                const double o1 = (sin (o1_angle * 2.0 * double_Pi));
                const double amplitude = 1.0 + (0.5 * o1);
                const float currentSampleVal = 
					(float) (wavetable.lookup(currentAngle) * levelMult * amplitude);
                
                for (int i = outputBuffer.getNumChannels(); --i >= 0;)
				{
                    *outputBuffer.getSampleData (i, sample) += currentSampleVal;
                }
                
                currentAngle = angleWrap(currentAngle + angleDelta);
                o1_angle = angleWrap(o1_angle + o1_angleDelta);
           
				if (playing == keyReleased)
				{
                     tailOff *= 0.99;
					 if (tailOff <= 0.005)
                     {
                        clearCurrentNote();
                        playing = notPlaying;
                        angleDelta = 0.0;
                        break;
                     }
				}
			}
        }
    }
    
private:
   // wrap an angle around. ASSUMES IT'S NOT GREATER THAN 2.
    double angleWrap(double angle) const
	{
       return (angle > 1.0 ? angle - 1.0 : angle);
    }
    
    enum PlayState
    {
        notPlaying = 0,
        keyHeld,
        keyReleased
    };
    
    PlayState playing;
	double angles[HARMONICS];
	double levels[HARMONICS];
    double currentAngle, angleDelta;
	double level, tailOff;
	// osc1
    static const int o1_freq = 2;
    double o1_angle, o1_angleDelta;
    // a wavetable
    WaveTable wavetable;
};

//==============================================================================
/** A demo synth sound that's just a basic sine wave.. */
class SineWaveSound : public SynthesiserSound
{
public:
    SineWaveSound() {}

    bool appliesToNote (const int /*midiNoteNumber*/)           { return true; }
    bool appliesToChannel (const int /*midiChannel*/)           { return true; }
};

//==============================================================================
/** A simple demo synth voice that just plays a sine wave.. */
class SineWaveVoice  : public SynthesiserVoice
{
public:
    SineWaveVoice()
        : angleDelta (0.0),
          tailOff (0.0)
    {
    }

    bool canPlaySound (SynthesiserSound* sound)
    {
        return dynamic_cast <SineWaveSound*> (sound) != 0;
    }

    void startNote (int midiNoteNumber, float velocity,
                    SynthesiserSound* /*sound*/, int /*currentPitchWheelPosition*/)
    {
        currentAngle = 0.0;
        level = velocity * 0.15;
        tailOff = 0.0;

        double cyclesPerSecond = MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        double cyclesPerSample = cyclesPerSecond / getSampleRate();

        angleDelta = cyclesPerSample * 2.0 * double_Pi;
    }

    void stopNote (bool allowTailOff)
    {
        if (allowTailOff)
        {
            // start a tail-off by setting this flag. The render callback will pick up on
            // this and do a fade out, calling clearCurrentNote() when it's finished.

            if (tailOff == 0.0) // we only need to begin a tail-off if it's not already doing so - the
                                // stopNote method could be called more than once.
                tailOff = 1.0;
        }
        else
        {
            // we're being told to stop playing immediately, so reset everything..

            clearCurrentNote();
            angleDelta = 0.0;
        }
    }

    void pitchWheelMoved (int /*newValue*/)
    {
        // can't be bothered implementing this for the demo!
    }

    void controllerMoved (int /*controllerNumber*/, int /*newValue*/)
    {
        // not interested in controllers in this case.
    }

    void renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
    {
        if (angleDelta != 0.0)
        {
            if (tailOff > 0)
            {
                while (--numSamples >= 0)
                {
                    const float currentSample = (float) (sin (currentAngle) * level * tailOff);

                    for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                        *outputBuffer.getSampleData (i, startSample) += currentSample;

                    currentAngle += angleDelta;
                    ++startSample;

                    tailOff *= 0.99;

                    if (tailOff <= 0.005)
                    {
                        clearCurrentNote();

                        angleDelta = 0.0;
                        break;
                    }
                }
            }
            else
            {
                while (--numSamples >= 0)
                {
                    const float currentSample = (float) (sin (currentAngle) * level);

                    for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                        *outputBuffer.getSampleData (i, startSample) += currentSample;

                    currentAngle += angleDelta;
                    ++startSample;
                }
            }
        }
    }

private:
    double currentAngle, angleDelta, level, tailOff;
};

const float defaultGain = 1.0f;
const float defaultDelay = 0.5f;

//==============================================================================
JuceDemoPluginAudioProcessor::JuceDemoPluginAudioProcessor()
    : delayBuffer (2, 12000)
{
    // Set up some default values..
    gain = defaultGain;
    delay = defaultDelay;

    lastUIWidth = 400;
    lastUIHeight = 200;

    lastPosInfo.resetToDefault();
    delayPosition = 0;

    // Initialise the synth...
    for (int i = 4; --i >= 0;)
        synth.addVoice (new P2Voice());   // These voices will play our custom sine-wave sounds..

    synth.addSound (new P2Sound());
}

JuceDemoPluginAudioProcessor::~JuceDemoPluginAudioProcessor()
{
	synth.setCurrentPlaybackSampleRate(getSampleRate());
	keyboardState.reset();
}

//==============================================================================
int JuceDemoPluginAudioProcessor::getNumParameters()
{
    return totalNumParams;
}

float JuceDemoPluginAudioProcessor::getParameter (int index)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    switch (index)
    {
        case gainParam:     return gain;
        case delayParam:    return delay;
        default:            return 0.0f;
    }
}

void JuceDemoPluginAudioProcessor::setParameter (int index, float newValue)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    switch (index)
    {
        case gainParam:     gain = newValue;  break;
        case delayParam:    delay = newValue;  break;
        default:            break;
    }
}

float JuceDemoPluginAudioProcessor::getParameterDefaultValue (int index)
{
    switch (index)
    {
        case gainParam:     return defaultGain;
        case delayParam:    return defaultDelay;
        default:            break;
    }

    return 0.0f;
}

const String JuceDemoPluginAudioProcessor::getParameterName (int index)
{
    switch (index)
    {
        case gainParam:     return "gain";
        case delayParam:    return "delay";
        default:            break;
    }

    return String::empty;
}

const String JuceDemoPluginAudioProcessor::getParameterText (int index)
{
    return String (getParameter (index), 2);
}

//==============================================================================
void JuceDemoPluginAudioProcessor::prepareToPlay (double sampleRate, int /*samplesPerBlock*/)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    synth.setCurrentPlaybackSampleRate (sampleRate);
    keyboardState.reset();
    delayBuffer.clear();
}

void JuceDemoPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    keyboardState.reset();
}

void JuceDemoPluginAudioProcessor::reset()
{
    // Use this method as the place to clear any delay lines, buffers, etc, as it
    // means there's been a break in the audio's continuity.
    delayBuffer.clear();
}

void JuceDemoPluginAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int numSamples = buffer.getNumSamples();
    int channel;
	
    // output buffers will initially be garbage, must be cleared:
    for (int i = 0; i < getNumOutputChannels(); ++i) {
        buffer.clear (i, 0, buffer.getNumSamples());
    }

    // Go through the incoming data, and apply our gain to it...
    for (channel = 0; channel < getNumInputChannels(); ++channel)
        buffer.applyGain (channel, 0, buffer.getNumSamples(), gain);

    // Now pass any incoming midi messages to our keyboard state object, and let it
    // add messages to the buffer if the user is clicking on the on-screen keys
    keyboardState.processNextMidiBuffer (midiMessages, 0, numSamples, true);

    // and now get the synth to process these midi events and generate its output.
    synth.renderNextBlock (buffer, midiMessages, 0, numSamples);

    // ask the host for the current time so we can display it...
    AudioPlayHead::CurrentPositionInfo newTime;

    if (getPlayHead() != nullptr && getPlayHead()->getCurrentPosition (newTime))
    {
        // Successfully got the current time from the host..
        lastPosInfo = newTime;
    }
    else
    {
        // If the host fails to fill-in the current time, we'll just clear it to a default..
        lastPosInfo.resetToDefault();
    }
	
}

//==============================================================================
AudioProcessorEditor* JuceDemoPluginAudioProcessor::createEditor()
{
    return new JuceDemoPluginAudioProcessorEditor (this);
}

//==============================================================================
void JuceDemoPluginAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // Here's an example of how you can use XML to make it easy and more robust:

    // Create an outer XML element..
    XmlElement xml ("MYPLUGINSETTINGS");

    // add some attributes to it..
    xml.setAttribute ("uiWidth", lastUIWidth);
    xml.setAttribute ("uiHeight", lastUIHeight);
    xml.setAttribute ("gain", gain);
    xml.setAttribute ("delay", delay);

    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary (xml, destData);
}

void JuceDemoPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    // This getXmlFromBinary() helper function retrieves our XML from the binary blob..
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr)
    {
        // make sure that it's actually our type of XML object..
        if (xmlState->hasTagName ("MYPLUGINSETTINGS"))
        {
            // ok, now pull out our parameters..
            lastUIWidth  = xmlState->getIntAttribute ("uiWidth", lastUIWidth);
            lastUIHeight = xmlState->getIntAttribute ("uiHeight", lastUIHeight);

            gain  = (float) xmlState->getDoubleAttribute ("gain", gain);
            delay = (float) xmlState->getDoubleAttribute ("delay", delay);
        }
    }
}

const String JuceDemoPluginAudioProcessor::getInputChannelName (const int channelIndex) const
{
    return String (channelIndex + 1);
}

const String JuceDemoPluginAudioProcessor::getOutputChannelName (const int channelIndex) const
{
    return String (channelIndex + 1);
}

bool JuceDemoPluginAudioProcessor::isInputChannelStereoPair (int /*index*/) const
{
    return true;
}

bool JuceDemoPluginAudioProcessor::isOutputChannelStereoPair (int /*index*/) const
{
    return true;
}

bool JuceDemoPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool JuceDemoPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool JuceDemoPluginAudioProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double JuceDemoPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JuceDemoPluginAudioProcessor();
}