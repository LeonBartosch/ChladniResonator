#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "dsp/RectChladniPattern.h"

//==============================================================================
// Editor mit zwei Slidern (m, n), einem "absolute"-Toggle
// und einer Chladni-Visualisierung.
class ChladniProcessorEditor : public juce::AudioProcessorEditor,
                               private juce::Timer
{
public:
    ChladniProcessorEditor (ChladniProcessor&);
    ~ChladniProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;

private:
    ChladniProcessor& audioProcessor;

    juce::Slider modeSlider;
    juce::Label  modeLabel;

    juce::Slider thetaSlider;
    juce::Label  thetaLabel;

    juce::Slider aSlider;
    juce::Label  aLabel;

    juce::Slider bSlider;
    juce::Label  bLabel;
    
    juce::Slider alphaSlider;
    juce::Label  alphaLabel;

    juce::Slider betaSlider;
    juce::Label  betaLabel;

    juce::Slider baseFreqSlider;
    juce::Label  baseFreqLabel;

    juce::Slider bridgeXSlider;
    juce::Label  bridgeXLabel;

    juce::Slider bridgeYSlider;
    juce::Label  bridgeYLabel;

    juce::ToggleButton absoluteButton;

    juce::Label infoLabel;

    juce::TextButton singleViewButton { "Single Mode" };
    juce::TextButton allModesButton   { "All Modes" };
        
    juce::Slider qSlider;
    juce::Label  qLabel;

    juce::Slider mixSlider;
    juce::Label  mixLabel;

    juce::Slider inGainSlider;
    juce::Label  inGainLabel;

    juce::Slider outGainSlider;
    juce::Label  outGainLabel;

    juce::Slider maxModesSlider;
    juce::Label  maxModesLabel;

    juce::Image singlePatternImage;
    juce::Image combinedPatternImage;

    std::unique_ptr<juce::SliderParameterAttachment> modeAttachment;
    std::unique_ptr<juce::SliderParameterAttachment> thetaAttachment;
    std::unique_ptr<juce::SliderParameterAttachment> aAttachment;
    std::unique_ptr<juce::SliderParameterAttachment> bAttachment;
    std::unique_ptr<juce::SliderParameterAttachment> alphaAttachment;
    std::unique_ptr<juce::SliderParameterAttachment> betaAttachment;
    std::unique_ptr<juce::SliderParameterAttachment> f11Attachment;
    std::unique_ptr<juce::SliderParameterAttachment> brxAttachment;
    std::unique_ptr<juce::SliderParameterAttachment> bryAttachment;
    std::unique_ptr<juce::SliderParameterAttachment> qAttachment;
    std::unique_ptr<juce::SliderParameterAttachment> mixAttachment;
    std::unique_ptr<juce::SliderParameterAttachment> maxModesAttachment;
    std::unique_ptr<juce::SliderParameterAttachment> inGainAttachment;
    std::unique_ptr<juce::SliderParameterAttachment> outGainAttachment;

    bool showCombinedView = false;

    juce::Rectangle<int> getImageArea() const;
    void updateSinglePattern();
    void updateCombinedPattern();

    void timerCallback() override; // from juce::Timer

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChladniProcessorEditor)
};