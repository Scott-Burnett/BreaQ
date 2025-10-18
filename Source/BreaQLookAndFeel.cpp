#pragma once
#include "BreaQLookAndFeel.h"
#include "ParameterNames.h"

//==============================================================================
OptionSlider::OptionSlider() : Slider() {
    labels = nullptr;
    numLabels = 0;
}

//==============================================================================
void OptionSlider::init(const juce::StringArray& labels, int numLabels) {
    this->labels = &labels;
    this->numLabels = numLabels;
}

//==============================================================================
static void DrawArc(
    juce::Slider* slider,
    juce::Graphics& g
) {
    const float rotaryStartAngle = slider->getRotaryParameters().startAngleRadians;
    const float rotaryEndAngle = slider->getRotaryParameters().endAngleRadians;

    auto bounds = slider->getLocalBounds();
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto lineW = 4.0f;
    auto arcRadius = radius - lineW * 0.5f;

    juce::Path arc;
    arc.addCentredArc(
        bounds.getCentreX(),
        bounds.getCentreY(),
        arcRadius,
        arcRadius,
        0.0f,
        rotaryStartAngle,
        rotaryEndAngle,
        true
    );

    g.setColour(Colours::transparentOrange3);
    g.strokePath(
        arc,
        juce::PathStrokeType(
            lineW,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded
        )
    );

    auto ang = juce::jmap((float)slider->getValue(),
        0.0f, 1.0f,
        rotaryStartAngle, rotaryEndAngle
    );

    juce::Path filledArc;
    filledArc.addCentredArc(
        bounds.getCentreX(),
        bounds.getCentreY(),
        arcRadius,
        arcRadius,
        0.0f,
        rotaryStartAngle,
        ang,
        true
    );

    g.setGradientFill({
        Colours::pink, bounds.getTopLeft().toFloat(),
        Colours::orange3, bounds.getBottomRight().toFloat(),
        false
        });

    g.strokePath(
        filledArc,
        juce::PathStrokeType(
            1.2f,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded
        )
    );
}

//==============================================================================
static void DrawLabels(
    // const juce::StringArray labels,
    //int numLabels,
    //int highlight,
    //bool doubleHighlightAtFull,
    OptionSlider* slider,
    juce::Graphics& g
) {
    if (slider->labels == nullptr ||
        slider->numLabels == 0) {
        return;
    }

    auto bounds = slider->getLocalBounds();
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;

    float rotaryStartAngle = slider->getRotaryParameters().startAngleRadians;
    float rotaryEndAngle = slider->getRotaryParameters().endAngleRadians;

    float sliderPos = slider->getValue();

    float pos = 0;
    float inc = 1.0f / (float)(slider->numLabels - 1);

    for (int i = 0; i < slider->numLabels; i++, pos += inc) {
        auto ang = juce::jmap(pos, 0.0f, 1.0f, rotaryStartAngle, rotaryEndAngle);
        auto c = bounds.getCentre().getPointOnCircumference(radius + 1.0f, ang);

        juce::Rectangle<float> r;
        const juce::String& str = (*slider->labels)[i];

        r.setSize(g.getCurrentFont().getStringWidthFloat(str), g.getCurrentFont().getStringWidthFloat(str));
        r.setCentre(c);

        if ((int)sliderPos == i) {
            g.setColour(Colours::orange3);
        }
        /*else if (highlight >= i) {
            g.setGradientFill({
                Colours::pink, bounds.getTopLeft().toFloat(),
                Colours::orange3, bounds.getBottomRight().toFloat(),
                false
            });
        }*/
        else {
            g.setColour(Colours::transparentOrange3);
        }

        g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
    }
}

//==============================================================================
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
    auto fillDiameter = arcRadius;

    juce::Rectangle fillRect = 
        juce::Rectangle<float>(fillDiameter, fillDiameter)
        .withCentre(bounds.getCentre())
    ;

    g.setGradientFill({
        Colours::transparentPink, fillRect.getTopLeft(),
        Colours::transparentBackground2, fillRect.getCentre(),
        false
        });

    g.fillEllipse(fillRect);

    auto centreToThumb = fillDiameter * 0.25f;
    auto thumbDiameter = centreToThumb * 1.0f;

    juce::Point<float> thumbStart = fillRect.getCentre().getPointOnCircumference(radius * 0.5f, toAngle);
    juce::Point<float> thumbEnd = fillRect.getCentre().getPointOnCircumference(radius * 0.3f, toAngle);

    g.setColour(Colours::orange3);

    g.drawEllipse(fillRect, lineW);

    g.drawLine(thumbStart.getX(), thumbStart.getY(), thumbEnd.getX(), thumbEnd.getY(), lineW * 2.0f);

    if (auto* optionSlider = dynamic_cast<OptionSlider*>(&slider)) {
        DrawLabels(optionSlider, g);
    }
    else {
        DrawArc(&slider, g);
    }
}

//==============================================================================
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
    }

    g.fillRect(fillRect);

    g.setGradientFill({
        Colours::orange3, fillRect.getTopLeft(),
        Colours::orange1, fillRect.getBottomRight(),
        false
        });

    g.drawRect(fillRect.reduced(lineW), lineW);
}