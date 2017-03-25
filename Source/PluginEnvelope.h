/*
  ==============================================================================

    PluginEnvelope.h
    Created: 21 Feb 2017 8:08:48am
    Author:  Jake

  ==============================================================================
*/

#ifndef PLUGINENVELOPE_H_INCLUDED
#define PLUGINENVELOPE_H_INCLUDED

class VocoderEnvelope
{
public:

	float GetPeak(float AbsoluteInput, float Peak);
	bool IsAttackFinished(bool AttackFinished, float OldPeak, float AbsoluteValue);
	float NextEnvelopeSample(float Peak, float OldPeak, float ReleaseSliderValue, float AttackSliderValue, bool AttackFinished);
};



#endif  // PLUGINENVELOPE_H_INCLUDED
