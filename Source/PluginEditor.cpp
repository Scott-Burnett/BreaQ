#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <JuceLibraryCode/JuceHeader.h>
#include "ParameterNames.h"

//==============================================================================
const juce::Colour Colours::aquamarine =        
    juce::Colour(73, 212, 157); // (49D49D);

const juce::Colour Colours::teaGreen =          
    juce::Colour(191, 215, 181); // (BFD7B5);

const juce::Colour Colours::springBud =         
    juce::Colour(231, 239, 197); // (E7EFC5);

const juce::Colour Colours::champagne =         
    juce::Colour(242, 221, 164); // (F2DDA4);

const juce::Colour Colours::citron =            
    juce::Colour(162, 159, 21); // (A29F15);

const juce::Colour Colours::terraCotta =        
    juce::Colour(226, 109, 92); // (E26D5C);

const juce::Colour Colours::background1 =       
    juce::Colour(13, 16, 22); // (0x0d1016);

const juce::Colour Colours::background2 =       
    juce::Colour(0, 1, 10); // (0x00010a);

const juce::Colour Colours::lilac =             
    juce::Colour(218, 173, 224); // (0xdaade0);

const juce::Colour Colours::pink =              
    juce::Colour(238, 104, 175); // (0xee68af);

const juce::Colour Colours::orange2 =           
    juce::Colour(245, 187, 108); // (0xf5bb6c);

const juce::Colour Colours::orange1 =           
    juce::Colour(253, 198, 138); // (0xfdc68a);

const juce::Colour Colours::orange3 =           
    juce::Colour(247, 203, 139); // (0xf7cb8b);

const juce::Colour Colours::transparentPink =   
    juce::Colour::fromRGBA(238, 104, 175, 100); // (0xee68af);

const juce::Colour Colours::transparentLilac =  
    juce::Colour::fromRGBA(218, 173, 224, 100); // (0xdaade0);

const juce::Colour Colours::transparentCitron = 
    juce::Colour::fromRGBA(162, 159, 21, 100); // (A29F15);

const juce::Colour Colours::grid1 =             
    juce::Colour::fromRGBA(41, 45, 54, 50); // (0x292d36);

const juce::Colour Colours::grid2 =             
    juce::Colour::fromRGBA(61, 66, 77, 50); // (0x3d424d);

const juce::Colour Colours::grid3 =             
    juce::Colour(179, 177, 173); // (0xb3b1ad);

//==============================================================================
static void initSlider (
    std::string sliderName,
    juce::Slider& slider,
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& sliderAttachment,
    juce::AudioProcessorValueTreeState& vts,
    juce::AudioProcessorEditor& editor
) {
    editor.addAndMakeVisible(slider);
    slider.setSliderStyle (
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );

    slider.setTextBoxStyle (
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );

    slider.hideTextBox(false);
    sliderAttachment.reset (
        new juce::AudioProcessorValueTreeState::SliderAttachment (
            vts, sliderName, slider
    ));

//     /*addAndMakeVisible(crossOverFrequencyLabel);
//     crossOverFrequencyLabel.setText(
//         "Cross Over Frequency", juce::dontSendNotification
//     );*/
}

//==============================================================================
static void initButton (
    std::string buttonName,
    juce::ToggleButton& button,
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>& buttonAttachment,
    juce::AudioProcessorValueTreeState& vts,
    juce::AudioProcessorEditor& editor
) {
    editor.addAndMakeVisible(button);

    buttonAttachment.reset (
        new juce::AudioProcessorValueTreeState::ButtonAttachment (
            vts, buttonName, button
    ));
}

//==============================================================================
void SliceEditor::init (
    int sliceNumber,
    juce::AudioProcessorValueTreeState& vts,
    juce::AudioProcessorEditor& editor
) {
    initSlider (
        ParameterNames::sliceProbability[sliceNumber], 
        probabilitySlider, 
        probabilitySliderAttachment, 
        vts, 
        editor
    );

    initSlider (
        ParameterNames::sliceLength[sliceNumber], 
        probabilitySlider, 
        probabilitySliderAttachment, 
        vts, 
        editor
    );

    initButton (
        ParameterNames::sliceEnabled[sliceNumber], 
        enabledButton, 
        enabledButtonAttachment, 
        vts, 
        editor
    );
}

//==============================================================================
void SliceEditor::resized(juce::Rectangle<int> bounds) {
    auto width = bounds.getWidth();
    auto lengthBounds = bounds.removeFromLeft(width * 0.33f);
    auto probabilityBounds = bounds.removeFromLeft(width * 0.33f);
    auto enabledBounds = bounds;

    lengthSlider.setBounds(lengthBounds);
    probabilitySlider.setBounds(probabilityBounds);
    enabledButton.setBounds(enabledBounds);
}

//==============================================================================
void SliceEditor::paint(juce::Graphics&) {
    
}

//==============================================================================
void StripEditor::init (
    int stripNumber,
    juce::AudioProcessorValueTreeState& vts,
    juce::AudioProcessorEditor& editor
) {
    initSlider (
        ParameterNames::stripProbability[stripNumber], 
        probabilitySlider, 
        probabilitySliderAttachment, 
        vts, 
        editor
    );

    initSlider (
        ParameterNames::stripGroup[stripNumber], 
        groupSlider, 
        groupSliderAttachment, 
        vts, 
        editor
    );

    initSlider (
        ParameterNames::stripNote[stripNumber], 
        noteSlider, 
        noteSliderAttachment, 
        vts, 
        editor
    );

    initButton (
        ParameterNames::stripEnabled[stripNumber], 
        enabledButton, 
        enabledButtonAttachment, 
        vts, 
        editor
    );

    initButton (
        ParameterNames::stripBypassed[stripNumber], 
        bypassedButton, 
        bypassedButtonAttachment, 
        vts, 
        editor
    );

    slices = new SliceEditor[numSlices];

    for (int i = 0; i < numSlices; i++) {
        slices[i].init(i, vts, editor);
    }
}

//==============================================================================
void StripEditor::resized (juce::Rectangle<int> bounds) {
    auto height = bounds.getHeight();
    auto width = bounds.getWidth();

    auto groupBounds = bounds.removeFromTop(height * 0.1f);
    auto probabilityBounds = bounds.removeFromTop(height * 0.25f);
    auto sliceBounds = bounds.removeFromTop(height * 0.5f);
    auto enabledBypassedBounds = bounds;
    auto enabledBounds = enabledBypassedBounds.removeFromLeft(width * 0.75f);
    auto bypassedBounds = enabledBypassedBounds;

    groupSlider.setBounds(groupBounds);
    probabilitySlider.setBounds(probabilityBounds);
    enabledButton.setBounds(enabledBounds);
    bypassedButton.setBounds(bypassedBounds);

    auto sliceHeight = sliceBounds.getWidth() / numSlices;
    for (int i = 0; i < numSlices; i++) {
        slices[i].resized(sliceBounds.removeFromTop(sliceHeight));
    }
}

//==============================================================================
void StripEditor::paint(juce::Graphics& g) {
    for (int i = 0; i < numSlices; i++) {
        slices[i].paint(g);
    }
}

//==============================================================================
BreaQAudioProcessorEditor::BreaQAudioProcessorEditor (
    BreaQAudioProcessor& p, 
    juce::AudioProcessorValueTreeState& vts,
    SpectrumAnalyzer& analyzer
) : 
    AudioProcessorEditor (&p), 
    audioProcessor (p) 
{
    setSize (970, 600);
    juce::LookAndFeel::setDefaultLookAndFeel(&breaQLookAndFeel);

    strips = new StripEditor[numStrips];

    for (int i = 0; i < numStrips; i++) {
        strips[i].init(i, vts, *this);
    }
}

//==============================================================================
BreaQAudioProcessorEditor::~BreaQAudioProcessorEditor() {
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

//==============================================================================
void BreaQAudioProcessorEditor::paint (juce::Graphics& g) {
    g.fillAll(Colours::background1);

    for (int i = 0; i < numStrips; i++) {
        strips[i].paint(g);
    }
}

//==============================================================================
void BreaQAudioProcessorEditor::resized () {
    auto bounds = getLocalBounds();

    const int mainMargin = 20;
    bounds.removeFromTop(mainMargin);
    bounds.removeFromRight(mainMargin);
    bounds.removeFromBottom(mainMargin);
    bounds.removeFromLeft(mainMargin);

    auto titleBounds = bounds.removeFromTop(bounds.getHeight() * 0.15f);
    auto groupBounds = bounds.removeFromTop(bounds.getHeight() * 0.4f);
    auto stripBounds = bounds;

    auto stripWidth = stripBounds.getWidth() / numStrips;
    for (int i = 0; i < numStrips; i++) {
        strips[i].resized(bounds.removeFromLeft(stripWidth));
    }
}