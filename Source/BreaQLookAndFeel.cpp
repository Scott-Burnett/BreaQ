#pragma once
#include "BreaQLookAndFeel.h"

void BreaQLookAndFeel::drawRotarySlider(
		juce::Graphics& g,
		int x,
		int y,
		int width,
		int height,
		float sliderPos,
		const float rotaryStartAngle,
		const float rotaryEndAngle,
		juce::Slider& slider) {
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    // auto lineW = juce::jmax(3.0f, radius * 0.3f);
    auto lineW = 1.2f;
    auto arcRadius = radius - lineW * 0.5f;

    juce::Path backgroundArc;
    backgroundArc.addCentredArc(bounds.getCentreX(),
        bounds.getCentreY(),
        arcRadius,
        arcRadius,
        0.0f,
        rotaryStartAngle,
        rotaryEndAngle,
        true
    );

    //g.setColour(transparentOrange3);
    //g.strokePath(
    //    backgroundArc, 
    //    juce::PathStrokeType(
    //        lineW * 3.0f, 
    //        juce::PathStrokeType::curved, 
    //        juce::PathStrokeType::rounded
    //    )
    //);

    g.setColour(orange3);
    g.strokePath(
        backgroundArc,
        juce::PathStrokeType(
            lineW,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded
        )
    );

    /*
    if (slider.isEnabled()) {
        juce::Path valueArc;
        valueArc.addCentredArc(bounds.getCentreX(),
            bounds.getCentreY(),
            arcRadius,
            arcRadius,
            0.0f,
            rotaryStartAngle,
            toAngle,
            true
        );

        g.setColour(orange3);
        g.strokePath(
            valueArc, 
            juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded)
        );
    }

    */

    /*
    juce::Point<float> thumbPoint(
        bounds.getCentreX() + arcRadius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
        bounds.getCentreY() + arcRadius * std::sin(toAngle - juce::MathConstants<float>::halfPi)
    );

    g.setColour(orange3);
    g.drawLine(
        bounds.getCentreX(),
        bounds.getCentreY(),
        thumbPoint.getX(),
        thumbPoint.getY(),
        lineW
    );

    g.fillEllipse(
        juce::Rectangle<float>(lineW, lineW)
        .withCentre(bounds.getCentre())
    );

    g.fillEllipse(
        juce::Rectangle<float>(lineW, lineW)
        .withCentre(thumbPoint)
    );

    */

    auto fillDiameter = 2.0f * (arcRadius - 6.0f * lineW);
    //auto thumbDiameter = centreToThumb * 0.3f;

    juce::Rectangle fillRect = juce::Rectangle<float>
        (fillDiameter, fillDiameter)
        .withCentre(bounds.getCentre())
        ;

    g.setGradientFill({
        transparentCitron, fillRect.getTopRight(),
        background1, fillRect.getCentre(),
        false
        });

    g.fillEllipse(fillRect);

    g.setGradientFill({
        transparentPink, fillRect.getTopLeft(),
        transparentBackground2, fillRect.getCentre(),
        false
        });

    g.fillEllipse(fillRect);

    auto centreToThumb = fillDiameter * 0.25f;
    auto thumbDiameter = centreToThumb * 1.0f;

    juce::Point<float> thumbPoint(
        bounds.getCentreX() + centreToThumb * std::cos(toAngle - juce::MathConstants<float>::halfPi),
        bounds.getCentreY() + centreToThumb * std::sin(toAngle - juce::MathConstants<float>::halfPi)
    );

    juce::Rectangle thumbRect = juce::Rectangle<float>
        (thumbDiameter, thumbDiameter)
        .withCentre(thumbPoint)
        ;

    g.setGradientFill({
        background1, thumbRect.getCentre(),
        transparentCitron, thumbRect.getBottomLeft(),
        false
        });

    g.fillEllipse(thumbRect);

    g.setGradientFill({
        transparentBackground2, thumbRect.getCentre(),
        transparentPink, thumbRect.getBottomRight(),
        false
        });

    g.fillEllipse(thumbRect);

    g.setColour(transparentOrange3);

    juce::Path fillOutline;
    fillOutline.addEllipse(fillRect);
    g.strokePath(
        fillOutline,
        juce::PathStrokeType(
            lineW * 2,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded
        )
    );

    juce::Path thumbOutline;
    thumbOutline.addEllipse(thumbRect);
    g.strokePath(
        thumbOutline,
        juce::PathStrokeType(
            lineW * 2,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded
        )
    );

}