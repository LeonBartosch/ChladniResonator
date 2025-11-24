#include "PluginEditor.h"
#include "dsp/RectChladniPattern.h"

//==============================================================================

ChladniProcessorEditor::ChladniProcessorEditor (ChladniProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (900, 800);

    // --- Mode slider ---
    modeSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    modeSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 60, 20);
    // modeSlider.setRange (1.0, 80.0, 1.0);      // modeIndex = 1..80
    addAndMakeVisible (modeSlider);

    modeLabel.setText ("Mode", juce::dontSendNotification);
    modeLabel.attachToComponent (&modeSlider, true);
    addAndMakeVisible (modeLabel);

    // --- Theta slider (mix of degenerate pairs) ---
    thetaSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    thetaSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 60, 20);
    // thetaSlider.setRange (-1.0, 1.0, 0.01);    // mix ∈ [-1,1]
    addAndMakeVisible (thetaSlider);

    thetaLabel.setText ("theta", juce::dontSendNotification);
    thetaLabel.attachToComponent (&thetaSlider, true);
    addAndMakeVisible (thetaLabel);

    // --- A slider () ---
    aSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    aSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 60, 20);
    // aSlider.setRange (0.1, 2.0, 0.01);
    addAndMakeVisible (aSlider);

    aLabel.setText ("A", juce::dontSendNotification);
    aLabel.attachToComponent (&aSlider, true);
    addAndMakeVisible (aLabel);

    // --- B slider () ---
    bSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    bSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 60, 20);
    // bSlider.setRange (0.1, 2.0, 0.01);
    addAndMakeVisible (bSlider);

    bLabel.setText ("B", juce::dontSendNotification);
    bLabel.attachToComponent (&bSlider, true);
    addAndMakeVisible (bLabel);

    // --- alpha slider () ---
    alphaSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    alphaSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 60, 20);
    // alphaSlider.setRange (0.00, 0.05, 0.001);
    addAndMakeVisible (alphaSlider);

    alphaLabel.setText ("alpha", juce::dontSendNotification);
    alphaLabel.attachToComponent (&alphaSlider, true);
    addAndMakeVisible (alphaLabel);

    // --- beta slider () ---
    betaSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    betaSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 60, 20);
    // betaSlider.setRange (0.00, 0.10, 0.001);
    addAndMakeVisible (betaSlider);

    betaLabel.setText ("beta", juce::dontSendNotification);
    betaLabel.attachToComponent (&betaSlider, true);
    addAndMakeVisible (betaLabel);

    // --- Base frequency slider (f_11) ---
    baseFreqSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    baseFreqSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 60, 20);
    // baseFreqSlider.setRange (50.0, 1000.0, 1.0);   // Hz
    baseFreqSlider.setSkewFactorFromMidPoint (220.0);
    addAndMakeVisible (baseFreqSlider);

    baseFreqLabel.setText ("f11 [Hz]", juce::dontSendNotification);
    baseFreqLabel.attachToComponent (&baseFreqSlider, true);
    addAndMakeVisible (baseFreqLabel);

    // --- Bridge X slider (0..1) ---
    bridgeXSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    bridgeXSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 60, 20);
    // bridgeXSlider.setRange (0.0, 1.0, 0.01);
    addAndMakeVisible (bridgeXSlider);

    bridgeXLabel.setText ("Bridge X", juce::dontSendNotification);
    bridgeXLabel.attachToComponent (&bridgeXSlider, true);
    addAndMakeVisible (bridgeXLabel);

    // --- Bridge Y slider (0..1) ---
    bridgeYSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    bridgeYSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 60, 20);
    // bridgeYSlider.setRange (0.0, 1.0, 0.01);
    addAndMakeVisible (bridgeYSlider);

    bridgeYLabel.setText ("Bridge Y", juce::dontSendNotification);
    bridgeYLabel.attachToComponent (&bridgeYSlider, true);
    addAndMakeVisible (bridgeYLabel);

    // --- Q slider (global) ---
    qSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    qSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 60, 20);
    // qSlider.setRange (2.0, 200.0, 1.0);     // typischer Q-Bereich
    qSlider.setSkewFactorFromMidPoint (20.0);
    addAndMakeVisible (qSlider);

    qLabel.setText ("Q", juce::dontSendNotification);
    qLabel.attachToComponent (&qSlider, true);
    addAndMakeVisible (qLabel);

    // --- Mix slider (0..100 %) ---
    // mixSlider.setRange (0.0, 1.0, 0.001);
    mixSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 60, 20);

    // Anzeige logisch (0–100%):
    mixSlider.textFromValueFunction = [] (double value)
    {
        return juce::String (value * 100.0, 1) + "%";
    };

    mixSlider.valueFromTextFunction = [] (const juce::String& text)
    {
        return text.getDoubleValue() / 100.0;
    };

    addAndMakeVisible (mixSlider);

    mixLabel.setText ("Mix %", juce::dontSendNotification);
    mixLabel.attachToComponent (&mixSlider, true);
    addAndMakeVisible (mixLabel);

    // --- Max Modes slider ---
    maxModesSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    maxModesSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 60, 20);
    // maxModesSlider.setRange (1.0, 80.0, 1.0);  // z.B. 1..80
    addAndMakeVisible (maxModesSlider);

    maxModesLabel.setText ("Max Modes", juce::dontSendNotification);
    maxModesLabel.attachToComponent (&maxModesSlider, true);
    addAndMakeVisible (maxModesLabel);

    // --- Input Gain (dB) ---
    inGainSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    inGainSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 60, 20);
    // inGainSlider.setRange (-24.0, 24.0, 0.1);
    addAndMakeVisible (inGainSlider);

    inGainLabel.setText ("In Gain [dB]", juce::dontSendNotification);
    inGainLabel.attachToComponent (&inGainSlider, true);
    addAndMakeVisible (inGainLabel);

    // --- Output Gain (dB) ---
    outGainSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    outGainSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 60, 20);
    // outGainSlider.setRange (-24.0, 24.0, 0.1);
    addAndMakeVisible (outGainSlider);

    outGainLabel.setText ("Out Gain [dB]", juce::dontSendNotification);
    outGainLabel.attachToComponent (&outGainSlider, true);
    addAndMakeVisible (outGainLabel);

    modeAttachment      = std::make_unique<juce::SliderParameterAttachment>(*audioProcessor.apvts.getParameter("MODE"),      modeSlider);
    thetaAttachment     = std::make_unique<juce::SliderParameterAttachment>(*audioProcessor.apvts.getParameter("THETA"),     thetaSlider);
    aAttachment         = std::make_unique<juce::SliderParameterAttachment>(*audioProcessor.apvts.getParameter("A"),         aSlider);
    bAttachment         = std::make_unique<juce::SliderParameterAttachment>(*audioProcessor.apvts.getParameter("B"),         bSlider);
    alphaAttachment     = std::make_unique<juce::SliderParameterAttachment>(*audioProcessor.apvts.getParameter("ALPHA"),     alphaSlider);
    betaAttachment      = std::make_unique<juce::SliderParameterAttachment>(*audioProcessor.apvts.getParameter("BETA"),      betaSlider);
    f11Attachment       = std::make_unique<juce::SliderParameterAttachment>(*audioProcessor.apvts.getParameter("F11"),       baseFreqSlider);
    brxAttachment       = std::make_unique<juce::SliderParameterAttachment>(*audioProcessor.apvts.getParameter("BRX"),       bridgeXSlider);
    bryAttachment       = std::make_unique<juce::SliderParameterAttachment>(*audioProcessor.apvts.getParameter("BRY"),       bridgeYSlider);
    qAttachment         = std::make_unique<juce::SliderParameterAttachment>(*audioProcessor.apvts.getParameter("Q"),         qSlider);
    mixAttachment       = std::make_unique<juce::SliderParameterAttachment>(*audioProcessor.apvts.getParameter("MIX"),       mixSlider);
    maxModesAttachment  = std::make_unique<juce::SliderParameterAttachment>(*audioProcessor.apvts.getParameter("MAXMODES"),  maxModesSlider);
    inGainAttachment    = std::make_unique<juce::SliderParameterAttachment>(*audioProcessor.apvts.getParameter("INGAIN"),    inGainSlider);
    outGainAttachment   = std::make_unique<juce::SliderParameterAttachment>(*audioProcessor.apvts.getParameter("OUTGAIN"),   outGainSlider);

    // --- Absolute toggle ---
    absoluteButton.setButtonText ("absolute");
    absoluteButton.setClickingTogglesState (true);
    absoluteButton.setToggleState (true, juce::dontSendNotification);
    addAndMakeVisible (absoluteButton);

    // --- Info label ---
    infoLabel.setJustificationType (juce::Justification::centredLeft);
    infoLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (infoLabel);

    // Initial single-mode pattern image
    singlePatternImage = juce::Image (juce::Image::RGB, 256, 256, false);
    combinedPatternImage = juce::Image (juce::Image::RGB, 256, 256, false);

    // View buttons (like tabs)
    singleViewButton.setClickingTogglesState (true);
    allModesButton.setClickingTogglesState (true);

    singleViewButton.setRadioGroupId (1);
    allModesButton.setRadioGroupId (1);

    singleViewButton.setToggleState (true, juce::dontSendNotification);
    allModesButton.setToggleState (false, juce::dontSendNotification);

    addAndMakeVisible (singleViewButton);
    addAndMakeVisible (allModesButton);

    singleViewButton.onClick = [this]
    {
        showCombinedView = false;
        // einmal Single-Pattern aktualisieren
        updateSinglePattern();
        repaint();
    };

    allModesButton.onClick = [this]
    {
        showCombinedView = true;
        // evtl. direkt erstes Combined-Bild rendern
        updateCombinedPattern();
        repaint();
    };

    // Callbacks: whenever something changes → update + repaint
    auto controlsChanged = [this]
    {
        updateSinglePattern();
        repaint();
    };

    modeSlider.onValueChange      = controlsChanged;
    thetaSlider.onValueChange     = controlsChanged;
    aSlider.onValueChange         = controlsChanged;
    bSlider.onValueChange         = controlsChanged;
    alphaSlider.onValueChange     = controlsChanged;
    betaSlider.onValueChange      = controlsChanged;
    baseFreqSlider.onValueChange  = controlsChanged;
    bridgeXSlider.onValueChange   = controlsChanged;
    bridgeYSlider.onValueChange   = controlsChanged;
    absoluteButton.onClick        = controlsChanged;
    qSlider.onValueChange         = controlsChanged;
    mixSlider.onValueChange       = controlsChanged;
    maxModesSlider.onValueChange  = controlsChanged;
    inGainSlider.onValueChange    = controlsChanged;
    outGainSlider.onValueChange   = controlsChanged;

    // First pattern
    updateSinglePattern();

    // Timer for All Modes view
    startTimerHz (20); // 30 FPS is usually enough
}

ChladniProcessorEditor::~ChladniProcessorEditor() = default;

//==============================================================================

void ChladniProcessorEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    juce::ColourGradient grad (juce::Colours::black, 0.0f, 0.0f,
                               juce::Colour (0xFF202020), 0.0f, bounds.getBottom(), false);
    g.setGradientFill (grad);
    g.fillAll();

    auto area = getLocalBounds().reduced (10);

    // Header
    auto header = area.removeFromTop (40);
    g.setColour (juce::Colours::white);
    g.setFont (juce::Font (juce::FontOptions().withHeight (20.0f)));
    g.drawFittedText ("Chladni Resonator",
                      header,
                      juce::Justification::centredLeft,
                      1);

    // Left panel + right panel
    auto leftPanel  = area.removeFromLeft (320);
    auto rightPanel = area.reduced (10);

    // Left panel background
    g.setColour (juce::Colour (0xFF151515));
    g.fillRoundedRectangle (leftPanel.toFloat(), 8.0f);
    g.setColour (juce::Colours::grey);
    g.drawRoundedRectangle (leftPanel.toFloat(), 8.0f, 1.0f);

    // Right panel background
    g.setColour (juce::Colour (0xFF151515));
    g.fillRoundedRectangle (rightPanel.toFloat(), 8.0f);
    g.setColour (juce::Colours::grey);
    g.drawRoundedRectangle (rightPanel.toFloat(), 8.0f, 1.0f);

    // Draw image
    auto imageArea = getImageArea();

    if (showCombinedView)
    {
        if (combinedPatternImage.isValid())
        {
            g.drawImageWithin (combinedPatternImage,
                               imageArea.getX(), imageArea.getY(),
                               imageArea.getWidth(), imageArea.getHeight(),
                               juce::RectanglePlacement::centred);
        }
    }
    else
    {
        if (singlePatternImage.isValid())
        {
            g.drawImageWithin (singlePatternImage,
                               imageArea.getX(), imageArea.getY(),
                               imageArea.getWidth(), imageArea.getHeight(),
                               juce::RectanglePlacement::centred);
        }
    }

    // Bridge marker only in Single view (optional)
    const float xBridge = (float) bridgeXSlider.getValue();
    const float yBridge = (float) bridgeYSlider.getValue();

    float px = imageArea.getX() + xBridge * imageArea.getWidth();
    float py = imageArea.getY() + yBridge * imageArea.getHeight();

    const float r = 6.0f;
    g.setColour (juce::Colours::red.withAlpha (0.9f));
    g.fillEllipse (px - r, py - r, r * 2.0f, r * 2.0f);

    g.setColour (juce::Colours::white);
    g.drawEllipse (px - r, py - r, r * 2.0f, r * 2.0f, 1.0f);
}

void ChladniProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (10);

    area.removeFromTop (40); // header

    auto leftPanel  = area.removeFromLeft (320);
    auto rightPanel = area.reduced (10);
    juce::ignoreUnused (rightPanel); // used only in paint()

    leftPanel = leftPanel.reduced (15);

    // Mode slider
    auto row = leftPanel.removeFromTop (40);
    modeSlider.setBounds (row.removeFromRight (row.getWidth() - 60));

    // Theta slider
    row = leftPanel.removeFromTop (40);
    thetaSlider.setBounds (row.removeFromRight (row.getWidth() - 60));

    // A slider
    row = leftPanel.removeFromTop (40);
    aSlider.setBounds (row.removeFromRight (row.getWidth() - 60));

    // B slider
    row = leftPanel.removeFromTop (40);
    bSlider.setBounds (row.removeFromRight (row.getWidth() - 60));

    // Alpha slider
    row = leftPanel.removeFromTop (40);
    alphaSlider.setBounds (row.removeFromRight (row.getWidth() - 60));

    // Beta slider
    row = leftPanel.removeFromTop (40);
    betaSlider.setBounds (row.removeFromRight (row.getWidth() - 60));

    // Base frequency slider
    row = leftPanel.removeFromTop (40);
    baseFreqSlider.setBounds (row.removeFromRight (row.getWidth() - 60));

    // Bridge X slider
    row = leftPanel.removeFromTop (40);
    bridgeXSlider.setBounds (row.removeFromRight (row.getWidth() - 60));

    // Bridge Y slider
    row = leftPanel.removeFromTop (40);
    bridgeYSlider.setBounds (row.removeFromRight (row.getWidth() - 60));

    // Absolute toggle
    row = leftPanel.removeFromTop (30);
    absoluteButton.setBounds (row.removeFromLeft (140));

    // Q slider
    row = leftPanel.removeFromTop (40);
    qSlider.setBounds (row.removeFromRight (row.getWidth() - 60));

    // Mix slider
    row = leftPanel.removeFromTop (40);
    mixSlider.setBounds (row.removeFromRight (row.getWidth() - 60));

    row = leftPanel.removeFromTop (40);
    maxModesSlider.setBounds (row.removeFromRight (row.getWidth() - 60));

    row = leftPanel.removeFromTop (40);
    inGainSlider.setBounds (row.removeFromRight (row.getWidth() - 60));

    row = leftPanel.removeFromTop (40);
    outGainSlider.setBounds (row.removeFromRight (row.getWidth() - 60));

    // View buttons (Tabs) unten im linken Panel, z.B.
    auto tabRow = leftPanel.removeFromTop (30);
    singleViewButton.setBounds (tabRow.removeFromLeft (tabRow.getWidth() / 2).reduced (2));
    allModesButton.setBounds   (tabRow.reduced (2));

    // Info label
    leftPanel.removeFromTop (10);
    auto infoArea = leftPanel.removeFromTop (50);
    infoLabel.setBounds (infoArea);
}

//==============================================================================

void ChladniProcessorEditor::updateSinglePattern()
{
    const int   modeIndex = (int) modeSlider.getValue();
    const bool  absOn     = absoluteButton.getToggleState();
    const float theta0    = (float) thetaSlider.getValue(); // [-1,1]
    const float A         = (float) aSlider.getValue();
    const float B         = (float) bSlider.getValue();
    const float alpha     = (float) alphaSlider.getValue();
    const float beta      = (float) betaSlider.getValue();
    const float baseFreq  = (float) baseFreqSlider.getValue();
    const float xBridge   = (float) bridgeXSlider.getValue(); // 0..1
    const float yBridge   = (float) bridgeYSlider.getValue(); // 0..1

    const float theta = juce::jmap (theta0,
                                    -1.0f,  1.0f,
                                    -juce::MathConstants<float>::halfPi,
                                     juce::MathConstants<float>::halfPi);

    const float realTheta = RectChladniPattern::computeRealTheta (
        RectChladniPattern::modeIndexToMN (modeIndex).m,
        RectChladniPattern::modeIndexToMN (modeIndex).n,
        theta,
        A, B, alpha, beta
    );

    // Single-mode image
    singlePatternImage = RectChladniPattern::makeImageMixedAngle (modeIndex, realTheta, absOn, 256);

    // Mode info
    auto mn   = RectChladniPattern::modeIndexToMN (modeIndex);
    float rel = RectChladniPattern::relativeFreqFromMN (mn.m, mn.n);
    float absF = baseFreq * rel;

    // u at bridge position
    float u = RectChladniPattern::computeU (modeIndex, realTheta, xBridge, yBridge);

    juce::String text;
    text << "m=" << mn.m << ", n=" << mn.n
         << "   rel=" << juce::String (rel, 3)
         << "   f=" << juce::String (absF, 1) << " Hz"
         << "   u=" << juce::String (u, 3);

    infoLabel.setText (text, juce::dontSendNotification);
}

void ChladniProcessorEditor::updateCombinedPattern()
{
    const int res = 128;

    if (! combinedPatternImage.isValid()
        || combinedPatternImage.getWidth() != res
        || combinedPatternImage.getHeight() != res)
    {
        combinedPatternImage = juce::Image (juce::Image::RGB, res, res, false);
    }

    const auto& modes = audioProcessor.getModalModes();
    const int Nuse = audioProcessor.getNumModes();
    const int N = std::min (Nuse, (int) modes.size());

    for (int i = 0; i < N; ++i)
    {
        float env = modes[(size_t) i].env;
        // in Summe einrechnen
    }

    if (modes.empty())
    {
        combinedPatternImage.clear (combinedPatternImage.getBounds(), juce::Colours::black);
        return;
    }

    const float theta0  = (float) thetaSlider.getValue();
    const float theta = juce::jmap (theta0,
                                    -1.0f,  1.0f,
                                    -juce::MathConstants<float>::halfPi,
                                     juce::MathConstants<float>::halfPi);

    const float A     = (float) aSlider.getValue();
    const float B     = (float) bSlider.getValue();
    const float alpha = (float) alphaSlider.getValue();
    const float beta  = (float) betaSlider.getValue();

    const bool absOn = absoluteButton.getToggleState();

    for (int y = 0; y < res; ++y)
    {
        const float yy = (float) y / (float) (res - 1);

        for (int x = 0; x < res; ++x)
        {
            const float xx = (float) x / (float) (res - 1);

            float sum = 0.0f;

            for (size_t i = 0; i < modes.size(); ++i)
            {
                int modeIndex = (int) i + 1;
                auto mn = RectChladniPattern::modeIndexToMN (modeIndex);

                const float realTheta = RectChladniPattern::computeRealTheta (
                    mn.m, mn.n,
                    theta,
                    A, B, alpha, beta
                );

                float v = RectChladniPattern::W_mixedAngle (mn.m, mn.n, xx, yy, realTheta);

                if (absOn)
                    v = std::abs (v);   // hier absolute-Mode für All-Modes

                float env = modes[i].env;   // Stärke der Mode
                sum += env * v;
            }

            // einfache Normalisierung: ein bisschen runter skalieren und in [0,1] mappen
            float norm;
            if (absOn)
            {
                // bei |v| sind wir eher im Bereich [0,∞), also Clip + Scale
                norm = juce::jlimit (0.0f, 1.0f, sum * 4.0f); // 4.0f = heuristischer Faktor
            }
            else
            {
                // signed: [-?, ?] auf [0,1] mappen
                norm = 0.5f * (sum * 0.5f + 1.0f);
                norm = juce::jlimit (0.0f, 1.0f, norm);
            }

            auto c = (juce::uint8) (norm * 255.0f);
            combinedPatternImage.setPixelAt (x, y, juce::Colour (c, c, c));
        }
    }
}

void ChladniProcessorEditor::timerCallback()
{
    if (showCombinedView)
    {
        updateCombinedPattern();
        repaint();
    }
}

void ChladniProcessorEditor::mouseDown (const juce::MouseEvent& e)
{
    auto imageArea = getImageArea();

    if (imageArea.contains (e.position.toInt()))
    {
        // Normalised coordinates in [0,1]
        float xNorm = (e.position.x - (float) imageArea.getX()) / (float) imageArea.getWidth();
        float yNorm = (e.position.y - (float) imageArea.getY()) / (float) imageArea.getHeight();

        xNorm = juce::jlimit (0.0f, 1.0f, xNorm);
        yNorm = juce::jlimit (0.0f, 1.0f, yNorm);

        bridgeXSlider.setValue (xNorm, juce::sendNotificationSync);
        bridgeYSlider.setValue (yNorm, juce::sendNotificationSync);
    }
}

void ChladniProcessorEditor::mouseDrag (const juce::MouseEvent& e)
{
    // Allow dragging the bridge point as well
    mouseDown (e);
}

juce::Rectangle<int> ChladniProcessorEditor::getImageArea() const
{
    auto area = getLocalBounds().reduced (10);

    // Header (40 px)
    area.removeFromTop (40);

    // Controls + „tab“-Buttons links
    area.removeFromLeft (320);

    // Right panel
    auto rightPanel = area.reduced (10);

    // Image area inside right panel
    return rightPanel.reduced (20);
}