/*
  ==============================================================================

    PluginEnvelope.cpp
    Created: 21 Feb 2017 8:08:48am
    Author:  Jake

  ==============================================================================
*/

#include "PluginEnvelope.h"

float VocoderEnvelope::GetPeak(float AbsoluteInput, float Peak)
{
	// Set a new peak value if a new peak is detected
	if (AbsoluteInput > Peak)
	{
		Peak = AbsoluteInput;
	}
	return Peak;
}

bool VocoderEnvelope::IsAttackFinished(bool AttackFinished, float OldPeak, float AbsoluteValue)
{
	// Function used to determine thee envelopes currnt stage.
	// Attack will have finished once the current level is >= to the current peak.
	if (OldPeak >= AbsoluteValue)
	{
		AttackFinished = true;
	}
	else
	{
		AttackFinished = false;
	}

	return AttackFinished;
}

float VocoderEnvelope::NextEnvelopeSample(float Peak, float OldPeak, float ReleaseSliderValue, float AttackSliderValue, bool AttackFinished)
{			// If attack has finsihed, Envelope will transition from the current peak to the old peak (Release)
	if (AttackFinished == true)	
	{		
		Peak = ((Peak * (1 - ReleaseSliderValue) + OldPeak * ReleaseSliderValue));
	}
	else    // else, Envelope will transition from the current peak to the new peak (Attack)
	{
		Peak = ((OldPeak * (0.2 - AttackSliderValue) + Peak * (0.2 - AttackSliderValue)));
	}

	OldPeak = Peak;	// Relevent envelope modifier
	Peak = 0.0;		// Reset peak so a new one can be detected

	return OldPeak;
}
