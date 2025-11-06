#pragma once
#include <../JuceLibraryCode/JuceHeader.h>

//==============================================================================
/*
*/
class OptionSlider : public juce::Slider {
public:
	OptionSlider();

	void init(const juce::StringArray& labels, int numLabels);

	const juce::StringArray* labels;
	int numLabels;
};

//==============================================================================
/*
*/
class BreaQLookAndFeel : public juce::LookAndFeel_V4
{
public:
	void drawRotarySlider(
		juce::Graphics& g, 
		int x, 
		int y, 
		int width, 
		int height, 
		float sliderPos,
		const float rotaryStartAngle, 
		const float rotaryEndAngle, 
		juce::Slider& slider
	);

	void drawToggleButton(
		juce::Graphics&,
		juce::ToggleButton&,
		bool shouldDrawButtonAsHighlighted,
		bool shouldDrawButtonAsDown
	);
};