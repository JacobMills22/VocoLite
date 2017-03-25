/*
==============================================================================

This file was auto-generated!

It contains the basic framework code for a JUCE plugin processor.

==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PluginEnvelope.h"

//==============================================================================
VocoderVstAudioProcessor::VocoderVstAudioProcessor()
	:LogFile("C://Users/Jake/Documents/Coding/VocoderVST/Vocoder_Debug_Log.txt"), Logger(LogFile, "Welcome to the Log", 0)	// Different way to do this?

{
}

VocoderVstAudioProcessor::~VocoderVstAudioProcessor()
{
}

//==============================================================================
const String VocoderVstAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool VocoderVstAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool VocoderVstAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

double VocoderVstAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int VocoderVstAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int VocoderVstAudioProcessor::getCurrentProgram()
{
	return 0;
}

void VocoderVstAudioProcessor::setCurrentProgram(int index)
{
}

const String VocoderVstAudioProcessor::getProgramName(int index)
{
	return String();
}

void VocoderVstAudioProcessor::changeProgramName(int index, const String& newName)
{
}

//==============================================================================
void VocoderVstAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	// Use this method as the place to do any pre-playback
	// initialisation that you need..

	MeasureLength = (int32)(0.020 * sampleRate);
	SampleRateForGUI = sampleRate;
	
	CalculateFilterFrequencies();
	UpdateVocoderFilters(sampleRate);
	
	// Reset All Filters
	for (int BandCount = 0; BandCount < VocoderBands; BandCount++)
	{
		for (int Channel = 0; Channel < 2; Channel++)
		{
			BandData* BandData = &BCD[BandCount][Channel];
			BandData->CarrierFilter.reset();
			BandData->ModultorFilter.reset();
		}
	}
		HighPassFilter.reset();
}

void VocoderVstAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VocoderVstAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

void VocoderVstAudioProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	const int totalNumInputChannels = getTotalNumInputChannels();
	const int totalNumOutputChannels = getTotalNumOutputChannels();
	// In case we have more outputs than inputs, this code clears any output
	// channels that didn't contain input data, (because these aren't
	// guaranteed to be empty - they may contain garbage).
	// This is here to avoid people getting screaming feedback
	// when they first compile a plugin, but obviously you don't need to keep
	// this code if your algorithm always overwrites all the output channels.

	for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	const float* fInput0 = buffer.getReadPointer(0);
	const float* fInput1 = buffer.getReadPointer(1);

	float Carrier, Modulator, VocodedOutput = 0.0;

	float* fOutputL = buffer.getWritePointer(0);
	float* fOutputR = buffer.getWritePointer(1);

	
	//	~~~~~~~~~~~~~~~ PROCESS SINGLE SAMPLE ~~~~~~~~~~~~~~~

	for (int sample = 0; sample < buffer.getNumSamples(); sample++)
	{
		// Left Channel = Carrier. Right Channel = Modulator
		Carrier = fInput0[sample] * CarrierLevelSliderValue;
		Modulator = fInput1[sample] * ModLevelSliderValue;

		//	~~~~~~~~~~~~~~~ PROCESS ALL BAND DATA ~~~~~~~~~~~~~~~

		for (int BandCount = 0; BandCount < VocoderBands; BandCount++)
		{
			for (int Channel = 0; Channel < 2; Channel++)
			{
				// Reference Band Channel Data
				BandData* BandData = &BCD[BandCount][Channel];
				memset(BandData, 0, sizeof(BandData));

				// Process Modulator Filters and Convert to Absolute Envelope Follower 
				BandData->FilteredModulator = BandData->ModultorFilter.processSingleSampleRaw(Modulator);
				BandData->AbsoluteInput = fabs(BandData->FilteredModulator);
				if (BandData->AbsoluteInput > 1.0)
				{
					BandData->AbsoluteInput = 1.0;
				}
				
				//	~~~~~~~~~~~~~~~ PROCESS ENVELOPE ~~~~~~~~~~~~~~~

				// Increment Sample Measure
				MeasuredSamples++;

				// Get Peak Value
				BandData->Peak = BandData->Envelope.GetPeak(BandData->AbsoluteInput, BandData->Peak);

				// Determine Envelope Stage
				BandData->AttackFinished = BandData->Envelope.IsAttackFinished(BandData->AttackFinished, BandData->OldPeak, BandData->AbsoluteInput);

				if (MeasuredSamples == MeasureLength)
				{
					MeasuredSamples = 0;
					// Calulate Atack and Release
					BandData->OldPeak = BandData->Envelope.NextEnvelopeSample(BandData->Peak, BandData->OldPeak,
						BandData->ReleaseSliderValue, BandData->AttackSliderValue, BandData->AttackFinished);

					BandData->BandForAnalyser = BandData->OldPeak;

					BandData->Peak = 0.0;

				}

				if (BandData->OldPeak > 1.0) { BandData->OldPeak = 1.0; }

			}
		}
		
		// Clear Output Before Adding Filters Together
		VocodedOutput = 0.0;

		

		// Process Carrier Filter Scaled by Envelope Follower, Modified by Release
		for (int Band = 0; Band < VocoderBands; Band++)
		{
			BandData* BandData = &BCD[Band][1];
			memset(BandData, 0, sizeof(BandData));
			VocodedOutput += BandData->CarrierFilter.processSingleSampleRaw(Carrier) * BandData->OldPeak;
		}

		

		// Scale Level Based on How Many Bands are Added Together
		VocodedOutput *= VocoderBandsLevelScale;

		// Process HighPass Filter
		VocodedOutput = HighPassFilter.processSingleSampleRaw(VocodedOutput);

		// Process LowPass Filter
		VocodedOutput = LowPassFilter.processSingleSampleRaw(VocodedOutput);

		// Process Sibilant Frequencies
		VocodedOutput = VocodedOutput + (CarrierHighPassFilter.processSingleSampleRaw(Modulator) * 0.005);
							
		// Final Output
		fOutputL[sample] = VocodedOutput * OutputSliderValue;
		fOutputR[sample] = VocodedOutput * OutputSliderValue;

	}


	
}

//==============================================================================
bool VocoderVstAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* VocoderVstAudioProcessor::createEditor()
{
	return new VocoderVstAudioProcessorEditor(*this);
}

//==============================================================================
void VocoderVstAudioProcessor::getStateInformation(MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.

}

void VocoderVstAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.

	
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new VocoderVstAudioProcessor();
}

// MY FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void VocoderVstAudioProcessor::CalculateFilterFrequencies()
{
	for (int Band = 0; Band < VocoderBands; Band++)
	{
		for (int Chan = 0; Chan < 2; Chan++)
		{
			BandData* BandData = &BCD[Band][Chan];
			memset(BandData, 0, sizeof(BandData));

			// Frequency =		   Minimum * power(Maxmum/Minimum, BandNumber/(VocoderBands - 1.0)
			// BandCenterFreq[Band] = 50.0 * pow(8000.0 / 50.0, Band / (16.0 - 1.0));
			BandData->BandFreq = 50.0 * pow(160, Band / (VocoderBands - 1.0));
		//	Logger.logMessage("Band Number: " + (String)Band + "  Centre Frequency is: " + (String)BandData->BandFreq);

		}
	}
}

void VocoderVstAudioProcessor::UpdateVocoderFilters(double sampleRate)
{
	const double BandPassQFactor = 10.0;

	// Set Coefficients, Update Parameters From Sliders and Initialise For All Filters.

	MeasuredSamples = 0;

	HighPassFilterParams = HighPassFilterParams.makeHighPass(sampleRate, HighPassFilterSliderValue);
	HighPassFilter.setCoefficients(HighPassFilterParams);

	LowPassFilterParams = LowPassFilterParams.makeLowPass(sampleRate, LowPassFilterSliderValue);
	LowPassFilter.setCoefficients(LowPassFilterParams);

	CarrierHighPassFilterParams = CarrierHighPassFilterParams.makeHighPass(sampleRate, 17000);
	CarrierHighPassFilter.setCoefficients(CarrierHighPassFilterParams);

	for (int Band = 0; Band < VocoderBands; Band++)
	{
		for (int Chan = 0; Chan < 2; Chan++)
		{		
			BandData* BandData = &BCD[Band][Chan];
			//memset(BandData, 0, sizeof(BandData));
			
			BandData->CarrierFilterParams = BandData->CarrierFilterParams.makeBandPass(sampleRate, BandData->BandFreq * BandData->FreqShiftSliderValue, BandPassQFactor * BandData->QFactorScale );
			BandData->CarrierFilter.setCoefficients(BandData->CarrierFilterParams);
			
			BandData->ModulatorFilterParams = BandData->ModulatorFilterParams.makeBandPass(sampleRate, BandData->BandFreq * BandData->FreqShiftSliderValue , BandPassQFactor * BandData->QFactorScale);
			BandData->ModultorFilter.setCoefficients(BandData->ModulatorFilterParams);

			BandData->Peak = 0.0;
			BandData->OldPeak = 0.0;

		}
	}

}


