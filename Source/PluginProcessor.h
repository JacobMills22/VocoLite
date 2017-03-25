/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginEnvelope.h"

#define MaxVocoderBands 32			// Defined in Processor.h, Processor.cpp and Editor.cpp



//==============================================================================
/**
*/
class VocoderVstAudioProcessor : public AudioProcessor
{
public:
	//==============================================================================
	VocoderVstAudioProcessor();
	~VocoderVstAudioProcessor();

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

	void processBlock(AudioSampleBuffer&, MidiBuffer&) override;

	//==============================================================================
	AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const String getProgramName(int index) override;
	void changeProgramName(int index, const String& newName) override;

	//==============================================================================
	void getStateInformation(MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	
	
	// BandData Structure, Contains Most Band-Related Variables and Objects
	struct BandData
	{
		float AbsoluteInput;
		IIRFilter CarrierFilter;
		IIRFilter ModultorFilter;
		IIRCoefficients CarrierFilterParams;
		IIRCoefficients ModulatorFilterParams;
		float FilteredModulator;
		float Peak;
		float OldPeak;
		bool AttackFinished;
		float VocodedOutput;
		double BandFreq;
		VocoderEnvelope Envelope;
		double BandForAnalyser;

		double FreqShiftSliderValue = 1.0;
		double QFactorScale = 1.0;
		double ReleaseSliderValue = 0.01;
		double AttackSliderValue = 0.01;
	};

	void UpdateVocoderFilters(double sampleRate);
	void CalculateFilterFrequencies();

	// VocoderBands
	int VocoderBands = 8;
	float VocoderBandsLevelScale = 1.0;
	
	// Slider Parameters
	double CarrierLevelSliderValue = 0.01, ModLevelSliderValue = 0.01, HighPassFilterSliderValue = 100.0, LowPassFilterSliderValue = 8000.0,
		OutputSliderValue = 0.1;

	// Measuring Variables
	int32 MeasureLength;
	int MeasuredSamples;

	int CountForLog = 0;

	//  SampleRate
	int SampleRateForGUI;

	int CurrentBandButton = 0;

	// High/Low PassFilter
	IIRFilter HighPassFilter, LowPassFilter, CarrierHighPassFilter;
	IIRCoefficients HighPassFilterParams, LowPassFilterParams, CarrierHighPassFilterParams;

	// Logging Objects
	File LogFile;
	FileLogger Logger;

	// BandData Object
	BandData BCD[MaxVocoderBands][2];
	
private:
    //==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VocoderVstAudioProcessor)

		
};



#endif  // PLUGINPROCESSOR_H_INCLUDED



// SOURCE 1