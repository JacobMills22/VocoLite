/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/

// ##############################################################################################
// ######################### VOCODER LOOK AND FEEL OVERRIDED FUNCTIONS ##########################
// ##############################################################################################

class VocoderLookAndFeel : public LookAndFeel_V3
{
public:

	void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
		const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override
	{
		const float Radius = jmin(width / 2, height / 2) - 4.0;
		const float CentreX = x + width * 0.5;
		const float CentreY = y + height * 0.5;
		const float Angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

		// Image (Encoder Background)
		Encoder = Encoder.rescaled(60, 60);
		g.drawImageAt(Encoder, x + 5, y - 5);
		
		// Pointer
		Path PointerPath;
		PointerPath.addEllipse(-2.0, -Radius + 2, 5.0, 5.0);
		PointerPath.applyTransform(AffineTransform::rotation(Angle).translated(CentreX, CentreY));

		g.setColour(Colours::darkslategrey);
		g.fillPath(PointerPath);
	}

	void drawLinearSliderBackground(Graphics& g, int x, int y, int width, int height, float sliderPos,
		float minSliderPos, float maxSliderPos, const Slider::SliderStyle style, Slider& slider) override
	{
		SliderBackground = SliderBackground.rescaled(20, 150);
		g.setOpacity(1.0f);
		g.drawImageAt(SliderBackground, x, y);
	}

	void drawLinearSliderThumb(Graphics& g, int x, int y, int width, int height, float sliderPos,
		float minSliderPos, float maxSliderPos, const Slider::SliderStyle style, Slider& slider) override
	{
		SliderThumb = SliderThumb.rescaled(20, 10);
		g.drawImageAt(SliderThumb, x, y + sliderPos - 10);
	}

	void drawButtonBackground(Graphics& g, Button& button, const Colour& backgroundColour,
		bool isMouseOverButton, bool isButtonDown) override
	{
		Rectangle<int> buttonArea = button.getLocalBounds();
		g.drawImageAt(ButtonLEDOn, buttonArea.getX() + 20, buttonArea.getY() + 6);
	}

	void drawToggleButton(Graphics& g, ToggleButton& button, bool isMouseOverButton, bool isButtonDown) override
	{
		Rectangle<int> buttonArea = button.getLocalBounds();
		g.drawImageAt(ButtonLEDOn, buttonArea.getX() + 20, buttonArea.getY() + 4);
	}

	Image Encoder = ImageCache::getFromMemory(BinaryData::Encoder70x70_png, BinaryData::Encoder70x70_pngSize);
	Image SliderBackground = ImageCache::getFromMemory(BinaryData::Slider_png, BinaryData::Slider_pngSize);
	Image SliderThumb = ImageCache::getFromMemory(BinaryData::SliderHandle_png, BinaryData::SliderHandle_pngSize);
	Image ButtonLEDOn = ImageCache::getFromMemory(BinaryData::LEDON_png, BinaryData::LEDON_pngSize);
};

// ##############################################################################################
// ################################## VOCODER AUDIO EDITIOR #####################################
// ##############################################################################################

class VocoderVstAudioProcessorEditor : public AudioProcessorEditor,
									   public Timer,
									   private Slider::Listener,
											   Button::Listener
{
public:
    VocoderVstAudioProcessorEditor (VocoderVstAudioProcessor&);
    ~VocoderVstAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
	void resized() override;

	void timerCallback();
	
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    VocoderVstAudioProcessor& processor;
	
	// Button Related Objects
	TextButton TotalBandsButton[3];
	void buttonClicked(Button* button) override;
	ToggleButton VocoderToggleButton[2];
	enum ToggleButtonParameter{AnalyserView, AboutView};
	
	int MainComponentWidth = 500, MainComponentHeight = 400;

	// Slider Related Objects
	Slider Rotary[7];
	enum RotaryParameter {HighPassFilter, LowPassFilter, OutputScale, Attack, Release, Qfactor, FreqShift};
	Slider LinearSlider[2];
	enum SliderParameter {CarrierGain, ModulatorGain};
	void sliderValueChanged(Slider* slider) override;

	void DrawAboutOverlay(Graphics& g);

	VocoderLookAndFeel VocoderVstLookAndFeel;

	Image Background = ImageCache::getFromMemory(BinaryData::VocoLiteBackground_png, BinaryData::VocoLiteBackground_pngSize);

	// Logging Objects
	File GUILogFile;
	FileLogger GUILogger;

	
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocoderVstAudioProcessorEditor)
};


// ##############################################################################################
// ################################## VOCODER ANALYSER ##########################################
// ##############################################################################################

class VocoderVstAnalyser
{
public:
	void DrawInitAnalyser(Graphics& g, float BarScale, int BandNumber, int TotalBands);
	
};

#endif  // PLUGINEDITOR_H_INCLUDED


// SOURCE 1