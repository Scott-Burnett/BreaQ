#pragma once
#include <JuceHeader.h>

class BreaQLookAndFeel : public juce::LookAndFeel_V4
{
public:
	// aquamarine: 49D49D
	// tea green: #BFD7B5
	// spring bud: #E7EFC5
	// champagne: #F2DDA4

	// citron: #A29F15
	// terra cotta: #E26D5C

	const juce::Colour aquamarine = juce::Colour(73, 212, 157); // (49D49D);
	const juce::Colour teaGreen = juce::Colour(191, 215, 181); // (BFD7B5);
	const juce::Colour springBud = juce::Colour(231, 239, 197); // (E7EFC5);
	const juce::Colour champagne = juce::Colour(242, 221, 164); // (F2DDA4);
	const juce::Colour citron = juce::Colour(162, 159, 21); // (A29F15);
	const juce::Colour transparentCitron = juce::Colour::fromRGBA(162, 159, 21, 100); // (A29F15);
	const juce::Colour terraCotta = juce::Colour(226, 109, 92); // (E26D5C);

	const juce::Colour background1 = juce::Colour(13, 16, 22); // (0x0d1016);
	const juce::Colour background2 = juce::Colour(0, 1, 10); // (0x00010a);
	const juce::Colour transparentBackground2 = juce::Colour::fromRGBA(0, 1, 10, 80); // (0x00010a);

	const juce::Colour lilac = juce::Colour(218, 173, 224); // (0xdaade0);

	const juce::Colour pink = juce::Colour(238, 104, 175); // (0xee68af);
	const juce::Colour transparentPink = juce::Colour::fromRGBA(238, 104, 175, 80); // (0xee68af);

	const juce::Colour grid1 = juce::Colour(41, 45, 54); // (0x292d36);
	const juce::Colour grid2 = juce::Colour(61, 66, 77); // (0x3d424d);
	const juce::Colour grid3 = juce::Colour(179, 177, 173); // (0xb3b1ad);

	const juce::Colour orange2 = juce::Colour(245, 187, 108); // (0xf5bb6c);
	const juce::Colour orange1 = juce::Colour(253, 198, 138); // (0xfdc68a);
	const juce::Colour orange3 = juce::Colour(247, 203, 139); // (0xf7cb8b);

	const juce::Colour transparentOrange3 = juce::Colour::fromRGBA(247, 203, 139, 30); // (0xf7cb8b);

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

private:
	const juce::Colour usedArcColour = 
		juce::Colour(255, 255, 255);
	const juce::Colour unusedArcColour = 
		juce::Colour(0, 0, 0);
};