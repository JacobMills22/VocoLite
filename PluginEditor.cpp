/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/


// Laptop		C:\Program Files (x86)\Image-Line\FL Studio 12\Plugins\VST
// Desktop		C:\Program Files (x86)\Image-Line\FL Studio 11\Plugins\VST

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
VocoderVstAudioProcessorEditor::VocoderVstAudioProcessorEditor (VocoderVstAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p), GUILogFile("C://Users/Jake/Documents/Coding/VocoderVST/Vocoder_Debug_Log.txt"), GUILogger(GUILogFile, "Welcome to the Log", 0)

{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1000, 400);

	//	~~~~~~~~~~~~~~~~ SLIDER PARAMETERS ~~~~~~~~~~~~~~~~

	for (int SliderNumber = 0; SliderNumber < 2; SliderNumber++)
	{
		LinearSlider[SliderNumber].addListener(this);
		addAndMakeVisible(LinearSlider[SliderNumber]);
		LinearSlider[SliderNumber].setRange(0.01, 1.0, 0.01);
		LinearSlider[SliderNumber].setSliderStyle(Slider::SliderStyle::LinearVertical);
		LinearSlider[SliderNumber].setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
		LinearSlider[SliderNumber].setLookAndFeel(&VocoderVstLookAndFeel);
	}

	//	~~~~~~~~~~~~~~~~ ROTARY PARAMETERS ~~~~~~~~~~~~~~~~
	
	for (int RotaryNumber = 0; RotaryNumber < 7; RotaryNumber++)
	{
		Rotary[RotaryNumber].addListener(this);
		addAndMakeVisible(Rotary[RotaryNumber]);
		Rotary[RotaryNumber].setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
		Rotary[RotaryNumber].setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
		Rotary[RotaryNumber].setLookAndFeel(&VocoderVstLookAndFeel);
	}

	Rotary[HighPassFilter].setRange		(20.0, 500.0, 5.0);
	Rotary[LowPassFilter].setRange		(500.0, 20000, 100.0);
	Rotary[OutputScale].setRange		(0.1, 60.0, 0.5);
	Rotary[Attack].setRange				(0.01, 0.15, 0.01);
	Rotary[Release].setRange			(0.01, 0.99, 0.01);
	Rotary[Qfactor].setRange			(0.5, 5.0, 0.01);
	Rotary[FreqShift].setRange			(0.1, 2.0, 0.01);

	//	~~~~~~~~~~~~~~~~ BUTTON PARAMETERS ~~~~~~~~~~~~~~~~
	
	// Toggle Buttons (Analyser & About)

	for (int ToggleButtonNum = 0; ToggleButtonNum < 2; ToggleButtonNum++)
	{
		addAndMakeVisible(VocoderToggleButton[ToggleButtonNum]);
		VocoderToggleButton[ToggleButtonNum].addListener(this);
		VocoderToggleButton[ToggleButtonNum].setLookAndFeel(&VocoderVstLookAndFeel);
		VocoderToggleButton[ToggleButtonNum].setToggleState(false, dontSendNotification);
		VocoderToggleButton[ToggleButtonNum].setAlpha(0);
	}
	
	// TextButtons (Band Number 8/16/32)
	for (int ButtonNumber = 0; ButtonNumber < 3; ButtonNumber++)
	{
		addAndMakeVisible(&TotalBandsButton[ButtonNumber]);
		TotalBandsButton[ButtonNumber].addListener(this);
		TotalBandsButton[ButtonNumber].setLookAndFeel(&VocoderVstLookAndFeel);
	}

	TotalBandsButton[0].setAlpha(100);
	TotalBandsButton[1].setAlpha(0);
	TotalBandsButton[2].setAlpha(0);

	startTimer(30);

	LinearSlider[CarrierGain].setValue(processor.CarrierLevelSliderValue, dontSendNotification);
	LinearSlider[ModulatorGain].setValue(processor.ModLevelSliderValue, dontSendNotification);

	Rotary[HighPassFilter].setValue(processor.HighPassFilterSliderValue, dontSendNotification);
	Rotary[LowPassFilter].setValue(processor.LowPassFilterSliderValue, dontSendNotification);
	Rotary[OutputScale].setValue(processor.OutputSliderValue, dontSendNotification);
	Rotary[Attack].setValue(processor.BCD[0][0].AttackSliderValue, dontSendNotification);
	Rotary[Release].setValue(processor.BCD[0][0].ReleaseSliderValue, dontSendNotification);
	Rotary[Qfactor].setValue(processor.BCD[0][0].QFactorScale, dontSendNotification);
	Rotary[FreqShift].setValue(processor.BCD[0][0].FreqShiftSliderValue, dontSendNotification);

	for (int ButtonNumber = 0; ButtonNumber < 3; ButtonNumber++)	{ TotalBandsButton[0].setAlpha(0); }
	TotalBandsButton[processor.CurrentBandButton].setAlpha(100);
}

VocoderVstAudioProcessorEditor::~VocoderVstAudioProcessorEditor()
{
}

//==============================================================================
void VocoderVstAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours:: grey);
	g.drawImageAt(Background, 0, 0);
	
	VocoderVstAnalyser Analyser;
	
	// If analyser button is on draw the analyser.
	if (VocoderToggleButton[AnalyserView].getToggleState() == true)
	{
		for (int BandCount = 0; BandCount < processor.VocoderBands; BandCount++)
		{
			Analyser.DrawInitAnalyser(g, processor.BCD[BandCount][1].BandForAnalyser, BandCount, processor.VocoderBands);
		}
	}

	// If about button is on draw the about view
	if (VocoderToggleButton[AboutView].getToggleState() == true)
	{
		DrawAboutOverlay(g);
	}
		
}

void VocoderVstAudioProcessorEditor::timerCallback()
{	
	repaint();
}

void VocoderVstAudioProcessorEditor::resized()
{
	setSize(MainComponentWidth, MainComponentHeight);

	//		SLIDER BOUNDS
	LinearSlider[CarrierGain].setBounds			(320, 130, 20, 150);
	LinearSlider[ModulatorGain].setBounds		(373, 130, 20, 150);

	//		ROTARY BOUNDS
	Rotary[HighPassFilter].setBounds			(106, 120, 70, 70);
	Rotary[LowPassFilter].setBounds				(106, 220, 70, 70);
	Rotary[OutputScale].setBounds				(415, 120, 70, 70);
	Rotary[Attack].setBounds					(212, 120, 70, 70);
	Rotary[Release].setBounds					(212, 220, 70, 70);
	Rotary[Qfactor].setBounds					(25, 120, 70, 70);
	Rotary[FreqShift].setBounds					(25, 220, 70, 70);

	//		BUTTON BOUNDS
	TotalBandsButton[0].setBounds				(60, 374, 100, 20);
	TotalBandsButton[1].setBounds				(142, 374, 100, 20);
	TotalBandsButton[2].setBounds				(220, 374, 100, 20);

	VocoderToggleButton[AnalyserView].setBounds	(320, 375, 64, 12);
	VocoderToggleButton[AboutView].setBounds	(410, 375, 63, 12);
}

void VocoderVstAudioProcessorEditor::sliderValueChanged(Slider* slider)
{	// Function finds the slider which is changed and sends data to the core
	if (slider == &Rotary[Qfactor] || slider == &Rotary[Release] || slider == &Rotary[Attack] || slider == &Rotary[FreqShift])
	{
		for (int BandCount = 0; BandCount < processor.VocoderBands; BandCount++)
		{
			for (int Channel = 0; Channel < 2; Channel++)
			{
				processor.BCD[BandCount][Channel].QFactorScale = Rotary[Qfactor].getValue();
				processor.BCD[BandCount][Channel].FreqShiftSliderValue = Rotary[FreqShift].getValue();
				processor.BCD[BandCount][Channel].AttackSliderValue = Rotary[Attack].getValue();
				processor.BCD[BandCount][Channel].ReleaseSliderValue = Rotary[Release].getValue();
			}
		}
	}

	else if (slider == &LinearSlider[CarrierGain]) { processor.CarrierLevelSliderValue = LinearSlider[CarrierGain].getValue();}  // LEFT
	else if (slider == &LinearSlider[ModulatorGain]) { processor.ModLevelSliderValue = LinearSlider[ModulatorGain].getValue(); } // RIGHT
	else if (slider == &Rotary[HighPassFilter]) { processor.HighPassFilterSliderValue = Rotary[HighPassFilter].getValue(); }
	else if (slider == &Rotary[LowPassFilter]) { processor.LowPassFilterSliderValue = Rotary[LowPassFilter].getValue(); }
	else if (slider == &Rotary[OutputScale]) { processor.OutputSliderValue = Rotary[OutputScale].getValue(); }

 	processor.UpdateVocoderFilters(processor.SampleRateForGUI);
}


void VocoderVstAudioProcessorEditor::buttonClicked(Button* button)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	BAND-RELATED BUTTONS	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 

	
	for (int ButtonNumber = 0; ButtonNumber < 3; ButtonNumber++)
	{
		if (button == &TotalBandsButton[ButtonNumber]) 
		{ 
			TotalBandsButton[ButtonNumber].setAlpha(100); // Turn on band button clicked
			processor.CurrentBandButton = ButtonNumber;	  // Variable used in core so that it can be stored by the DAW -
														  // allows the correct button to be turned on when VST is re-opened	
		}
		if (button != &VocoderToggleButton[AnalyserView] && button != &VocoderToggleButton[AboutView] && button != &TotalBandsButton[ButtonNumber]) 
		{
			TotalBandsButton[ButtonNumber].setAlpha(0);	// Turn off the band buttons which were not clicked
		}
	}

	// Logic to send data to core when the user changes the number of bands.
	if (button == &TotalBandsButton[0])
	{
		processor.VocoderBands = 8;	// sets the band number for the core to use.
		processor.VocoderBandsLevelScale = 2.0;	// Sets the scale value for the core to use.
	}											// (More bands that are added means the louder the output will be, and 
	else if (button == &TotalBandsButton[1])	// therefore needs scaling so the output level remains the same regardless of the number of bands used).
	{
		processor.VocoderBands = 16;
		processor.VocoderBandsLevelScale = 1.0;				
	}
	else if (button == &TotalBandsButton[2])
	{
		processor.VocoderBands = 32;
		processor.VocoderBandsLevelScale = 0.5;
	}

	if (button != &VocoderToggleButton[AnalyserView] && button != &VocoderToggleButton[AboutView])
	{
		processor.CalculateFilterFrequencies();						// Only update/calculate filter frequencies if a band button was pressed.
		processor.UpdateVocoderFilters(processor.getSampleRate());
	}

	//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	ANALYSER/ABOUT TOGGLE BUTTONS	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// Button Logic which allows only 1 Band button to be on at a time. Similar logic with the two extra buttons (Analyser and About)
	if (button == &VocoderToggleButton[AnalyserView] || button == &VocoderToggleButton[AboutView])	
	{
		// If Analyser button clicked to turn on
		if (button == &VocoderToggleButton[AnalyserView] && VocoderToggleButton[AnalyserView].getToggleState() == true)
		{
			VocoderToggleButton[AboutView].setToggleState(false, dontSendNotification);	
			VocoderToggleButton[AboutView].setAlpha(0);	     // Turn off AboutView button.
			VocoderToggleButton[AnalyserView].setAlpha(100); // Turn AnalyserView button on.
			MainComponentWidth = 1000;
			MainComponentHeight = 400;	// Resize component so the analyser can be seen.
			startTimer(50);				// Start a timer so the the analyser updates and a chnage in frequency can be seen.		
		}
		else if (button == &VocoderToggleButton[AboutView] && VocoderToggleButton[AboutView].getToggleState() == true)
		{
			VocoderToggleButton[AnalyserView].setToggleState(false, dontSendNotification);
			VocoderToggleButton[AnalyserView].setAlpha(0);
			VocoderToggleButton[AboutView].setAlpha(100);
			MainComponentWidth = 1000;
			MainComponentHeight = 400;	// Similar logic but reversed to above.
			stopTimer();				// Periodic updates are not needed to turn off (Analyser view not displayed)
		}
		else  // If buttons are both turned off
		{
			MainComponentWidth = 500;	// Set the component to a compact view
			MainComponentHeight = 400;
			stopTimer();				// Periodic updates are not needed to turn off (Analyser view not displayed)
			VocoderToggleButton[AnalyserView].setAlpha(0);
			VocoderToggleButton[AboutView].setAlpha(0);		// Turn both buttons off.
		}

		resized();
		repaint();	// If either buttons are pressed repaint the component.
	}
}


void VocoderVstAnalyser::DrawInitAnalyser(Graphics& g, float BarScale, int BandNumber, int TotalBands)
{
	int AnalyserLeftPos = 555;
	int AnalyserTopPos = 60;
	int XIncrement = 400 / TotalBands;
	int WidthScale = XIncrement - 2;
	int MaxBarHeight = 286;

	AnalyserLeftPos += XIncrement * BandNumber;
	int BarHeight = MaxBarHeight - (MaxBarHeight * (BarScale * 2) + 2) + 1;

		// Draw the whole blue Bar to be displayed at all times. (Widthscale - 2 so outline can be seen)
		g.setColour(Colours::dodgerblue);
		g.fillRect(AnalyserLeftPos, AnalyserTopPos, WidthScale - 2, MaxBarHeight);

		// Draw Left/Right outline ontop of blue bar
		g.setColour(Colours::black);
		g.drawRect(AnalyserLeftPos, AnalyserTopPos, WidthScale, MaxBarHeight);

		// Draw Top Outline
		g.setColour(Colours::black);
		g.drawRect(AnalyserLeftPos, AnalyserTopPos, WidthScale, BarHeight);

		// Draws another bar which matches the plugins background. Height is scaled based on Core meter data 
		// (gives the illusion that the blue bar is moving).  
		g.setColour(Colours::grey);
		g.fillRect(AnalyserLeftPos, AnalyserTopPos, WidthScale, BarHeight - 1);

}

void VocoderVstAudioProcessorEditor::DrawAboutOverlay(Graphics& g)
{
	g.setColour(Colours::lightslategrey);
	g.fillRect(500, 0, 500, 400);

	g.setColour(Colours::black);
	g.setFont(juce::Font::bold);
	g.setFont(18.0);
	g.drawSingleLineText("ABOUT VOCOLITE", 520, 50, Justification::left);

	g.setFont(14.0);
	g.drawSingleLineText("Beta Version V1.2 Release", 835, 15, Justification::left);

	g.setFont(juce::Font::plain);
	g.setFont(14.0);
	g.drawMultiLineText("VocoLite is a software vocoder utilising 32 bands and various other features." 
		"VocoLite does not have a built-in synthesiser, instead a modulator and carrier signal" 
		"must be manually passed to the plugin via a mixer insert/send", 505, 70, 500);


	g.setFont(juce::Font::bold);
	g.setFont(18.0);
	g.drawSingleLineText("INSTRUCTIONS", 520, 120, Justification::left);

	g.setFont(juce::Font::plain);
	g.setFont(14.0);
	g.drawMultiLineText("VocoLite takes a Carrier signal from its left channel and a modulator from its right channel."
						"Therefore both signals need to be sent to vocolite with the carrier (synthesiser) panned hard left"
						"and the modulator (speech) panned hard right", 505, 140, 500);

		g.setFont(juce::Font::bold);
	g.setFont(18.0);
	g.drawSingleLineText("FEATURE LIST", 520, 200, Justification::left);

	g.setFont(juce::Font::plain);
	g.setFont(14.0);
	g.drawMultiLineText(
		"Q (Width): Controls the frequency range of the bands, higher Q means tighter bands.\n"
		"Freq Shift: Offsets each bands frequncy range.\n"
		"High/LowPass: Adjusts the filter cutoff of the highpass/LowPass filter.\n"
		"Env (Envelope): The Envelope is used to smooth out the moduluated audio.\n"
		"Attack: The speed at which the envelope rises.\n"
		"Release: The speed at which the emvelope falls.\n"
		"Car/Mod: Control the level of the unmodulated signal.\n"
		"Gain: Controls the ouverall output level.\n"
		"Bands: Changes the number of bands that are used to modulate the carrier.\n"
		"Analyser: An analyser which displays the status of each band can be toggled on/off\n", 505, 220, 500);
	
	g.drawSingleLineText("VocoLite is powered by JUCE and developed by Jacob Mills.", 505, 390, Justification::left);

		
					
}
