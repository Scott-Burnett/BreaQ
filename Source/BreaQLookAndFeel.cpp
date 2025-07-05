#pragma once
#include "BreaQLookAndFeel.h"
#include "ParameterNames.h"

void BreaQLookAndFeel::drawRotarySlider(
		juce::Graphics& g,
		int x,
		int y,
		int width,
		int height,
		float sliderPos,
		const float rotaryStartAngle,
		const float rotaryEndAngle,
		juce::Slider& slider
) {
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(8);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight());// / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = 1.2f;
    auto arcRadius = radius - lineW * 0.5f;

    /*juce::Path backgroundArc;
    backgroundArc.addCentredArc(bounds.getCentreX(),
        bounds.getCentreY(),
        arcRadius,
        arcRadius,
        0.0f,
        rotaryStartAngle,
        rotaryEndAngle,
        true
    );

    g.setColour(orange3);
    g.strokePath(
        backgroundArc,
        juce::PathStrokeType(
            lineW,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded
        )
    );*/


    // ---------------------------------------------------

    //auto fillDiameter = 2.0f * (arcRadius - 6.0f * lineW);
    auto fillDiameter = arcRadius;

    juce::Rectangle fillRect = 
        juce::Rectangle<float>(fillDiameter, fillDiameter)
        .withCentre(bounds.getCentre())
    ;

    /*g.setGradientFill ({
        Colours::transparentCitron, fillRect.getTopRight(),
        Colours::background1, fillRect.getCentre(),
        false
    });

    g.fillEllipse(fillRect);*/

    g.setGradientFill({
        Colours::transparentPink, fillRect.getTopLeft(),
        Colours::transparentBackground2, fillRect.getCentre(),
        false
        });

    g.fillEllipse(fillRect);

    auto centreToThumb = fillDiameter * 0.25f;
    auto thumbDiameter = centreToThumb * 1.0f;

    juce::Point<float> thumbPoint(
        bounds.getCentreX() + centreToThumb * std::cos(toAngle - juce::MathConstants<float>::halfPi),
        bounds.getCentreY() + centreToThumb * std::sin(toAngle - juce::MathConstants<float>::halfPi)
    );

    juce::Rectangle thumbRect = 
        juce::Rectangle<float>(thumbDiameter * 0.2f, thumbDiameter * 0.2f)
        .withCentre(thumbPoint)
    ;

    /*g.setGradientFill ({
        Colours::background1, thumbRect.getCentre(),
        Colours::transparentCitron, thumbRect.getBottomLeft(),
        false
    });

    g.fillEllipse(thumbRect);*/

    /*g.setGradientFill ({
        Colours::transparentBackground2, thumbRect.getCentre(),
        Colours::transparentPink, thumbRect.getBottomRight(),
        false
    });

    g.fillEllipse(thumbRect);*/

    g.setColour(Colours::orange3);

    /*juce::Path fillOutline;
    fillOutline.addEllipse(fillRect);
    g.strokePath (
        fillOutline,
        juce::PathStrokeType(
            lineW * 2,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded
        )
    );*/

    g.drawEllipse(fillRect, lineW);

    /*juce::Path thumbOutline;
    thumbOutline.addEllipse(thumbRect);
    g.strokePath (
        thumbOutline,
        juce::PathStrokeType (
            lineW * 2,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded
        )
    );*/

    g.drawEllipse(thumbRect, lineW);
}

void BreaQLookAndFeel::drawToggleButton(
    juce::Graphics& g,
    juce::ToggleButton& button,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown
) {
    juce::Rectangle bounds = button.getLocalBounds().toFloat();
    float edge = juce::jmin(bounds.getWidth(), bounds.getHeight());

    auto lineW = 1.2f;

    juce::Rectangle fillRect = 
        juce::Rectangle(edge, edge)
        .withCentre(bounds.getCentre())
        .toFloat().reduced(2);

    if (button.getToggleState()) {
        g.setGradientFill({
            Colours::orange1, fillRect.getTopLeft(),
            Colours::orange3, fillRect.getBottomRight(),
            false
            });
    }
    else {
        g.setGradientFill({
            Colours::transparentOrange3, fillRect.getTopLeft(),
            Colours::transparentOrange1, fillRect.getBottomRight(),
            false
        });

        //g.setColour(Colours::transparentOrange3);
    }

    g.fillRect(fillRect);

    g.setGradientFill({
        Colours::orange3, fillRect.getTopLeft(),
        Colours::orange1, fillRect.getBottomRight(),
        false
        });

    g.drawRect(fillRect.reduced(lineW), lineW);


}