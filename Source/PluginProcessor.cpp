#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "dsp/RectChladniPattern.h"

//==============================================================================

ChladniProcessor::ChladniProcessor()
    : AudioProcessor (BusesProperties()
        #if ! JucePlugin_IsMidiEffect
        #if ! JucePlugin_IsSynth
            .withInput  ("Input", juce::AudioChannelSet::stereo(), true)
        #endif
            .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
        #endif
    ),
      apvts (*this, nullptr, "PARAMS", createParameterLayout())
{
    pF11     = apvts.getRawParameterValue("F11");
    pTheta   = apvts.getRawParameterValue("THETA");
    pA       = apvts.getRawParameterValue("A");
    pB       = apvts.getRawParameterValue("B");
    pAlpha   = apvts.getRawParameterValue("ALPHA");
    pBeta    = apvts.getRawParameterValue("BETA");
    pBridgeX = apvts.getRawParameterValue("BRX");
    pBridgeY = apvts.getRawParameterValue("BRY");
    pQ       = apvts.getRawParameterValue("Q");
    pMix     = apvts.getRawParameterValue("MIX");
    pModes   = apvts.getRawParameterValue("MAXMODES");
    pInGain  = apvts.getRawParameterValue("INGAIN");
    pOutGain = apvts.getRawParameterValue("OUTGAIN");
}

ChladniProcessor::~ChladniProcessor() = default;

//==============================================================================

const juce::String ChladniProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ChladniProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ChladniProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ChladniProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ChladniProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

//==============================================================================

int ChladniProcessor::getNumPrograms()
{
    return 1;
}

int ChladniProcessor::getCurrentProgram()
{
    return 0;
}

void ChladniProcessor::setCurrentProgram (int)
{
}

const juce::String ChladniProcessor::getProgramName (int)
{
    return {};
}

void ChladniProcessor::changeProgramName (int, const juce::String&)
{
}

//==============================================================================

void ChladniProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);
    const float f11 = *pF11;

    // Sample-Rate für beide Bodies und beide Limiter setzen
    for (int ch = 0; ch < 2; ++ch)
    {
        modalBodies[ch].setSampleRate (sampleRate);
        modalBodies[ch].setBaseFrequency (f11);
        limiters[ch].setSampleRate (sampleRate);
        limiters[ch].setThresholdDb (-1.0f);
        limiters[ch].setReleaseMs (50.0f);
    }

    const int maxModes = 80; // oder was du möchtest
    std::vector<ModalModeState> modes;
    modes.reserve ((size_t) maxModes);

    const float q = *pQ;

    for (int k = 1; k <= maxModes; ++k)
    {
        auto mn  = RectChladniPattern::modeIndexToMN (k);
        float rel  = RectChladniPattern::relativeFreqFromMN (mn.m, mn.n);

        ModalModeState m;
        m.modeIndex = k;
        m.m         = mn.m;
        m.n         = mn.n;
        m.relFreq   = rel;
        m.freqHz    = f11 * rel;
        m.q         = q;

        modes.push_back (m);
    }

    // dieselbe Modenliste in beide Bodies
    modalBodies[0].setModes (modes);
    modalBodies[1].setModes (modes);
}

void ChladniProcessor::releaseResources()
{
}

//==============================================================================

#if ! JucePlugin_PreferredChannelConfigurations
bool ChladniProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
   #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
   #else
    auto mainOut = layouts.getMainOutputChannelSet();
    if (mainOut != juce::AudioChannelSet::mono()
        && mainOut != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (mainOut != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
   #endif
}
#endif

void ChladniProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                     juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused (midi);

    const int numChannels = buffer.getNumChannels();
    const int numSamples  = buffer.getNumSamples();

    if (numChannels == 0)
        return;

    auto* left  = buffer.getWritePointer (0);
    auto* right = (numChannels > 1 ? buffer.getWritePointer (1) : nullptr);

    // Parameter aus Atomics
    const float f11 = *pF11;
    const float thetaMix = *pTheta;
    const float a = *pA;
    const float b = *pB;
    const float alpha = *pAlpha;
    const float beta = *pBeta;
    const float bx = *pBridgeX;
    const float by = *pBridgeY;
    const float q = *pQ;
    const float wet = *pMix;
    const int Nuse = *pModes;
    const float inDb = *pInGain;
    const float outDb = *pOutGain;

    const float dryGain = 1.0f - wet;
    const float wetGain = wet;

    const float inLin  = juce::Decibels::decibelsToGain (inDb);
    const float outLin = juce::Decibels::decibelsToGain (outDb);

    const float theta = juce::jmap (thetaMix,
                                    -1.0f,  1.0f,
                                    -juce::MathConstants<float>::halfPi,
                                    juce::MathConstants<float>::halfPi);

    // Q, Modenzahl & Kopplungen für beide Bodies setzen
    for (int ch = 0; ch < 2; ++ch)
    {
        modalBodies[ch].setBaseFrequency (f11);
        modalBodies[ch].setGlobalQ (q);
        modalBodies[ch].setActiveModeCount (Nuse);
        modalBodies[ch].updateCouplings (bx, by, theta, a, b, alpha, beta);
    }

    // Stereo-Loop: jeder Kanal durch seinen eigenen Body & Limiter
    for (int n = 0; n < numSamples; ++n)
    {
        const float inL = left[n];
        const float inR = (right != nullptr ? right[n] : inL); // falls mono: R = L

        // Input-Gain pro Kanal
        const float xinL = inL * inLin;
        const float xinR = inR * inLin;

        // Resonator pro Kanal
        const float bodyL = modalBodies[0].processSample (xinL);
        const float bodyR = modalBodies[1].processSample (xinR);

        // Dry/Wet pro Kanal
        const float yL = dryGain * xinL + wetGain * bodyL;
        const float yR = dryGain * xinR + wetGain * bodyR;

        // Output-Gain
        const float outL0 = yL * outLin;
        const float outR0 = yR * outLin;

        // Limiter pro Kanal
        const float limitedL = limiters[0].processSample (outL0);
        const float limitedR = limiters[1].processSample (outR0);

        left[n] = limitedL;
        if (right != nullptr)
            right[n] = limitedR;
    }

    // weitere Kanäle (falls >2) nullen
    for (int ch = 2; ch < numChannels; ++ch)
        buffer.clear (ch, 0, numSamples);
}

//==============================================================================

bool ChladniProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* ChladniProcessor::createEditor()
{
    return new ChladniProcessorEditor (*this);
}

//==============================================================================

void ChladniProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void ChladniProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));

    if (xml != nullptr && xml->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

//==============================================================================

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChladniProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout ChladniProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;

    p.push_back (std::make_unique<juce::AudioParameterFloat>(
        "MODE", "Mode",
        juce::NormalisableRange<float> (1.0f, 144.0f, 1.0f), 1.0f));

    p.push_back (std::make_unique<juce::AudioParameterFloat>(
        "THETA", "Theta",
        -1.0f, 1.0f, 0.0f));

    p.push_back (std::make_unique<juce::AudioParameterFloat>(
        "A", "A",
        0.0f, 2.0f, 1.0f));

    p.push_back (std::make_unique<juce::AudioParameterFloat>(
        "B", "B",
        -1.0f, 1.0f, 1.0f));
    
    p.push_back (std::make_unique<juce::AudioParameterFloat>(
        "ALPHA", "Alpha",
        0.000f, 0.050f, 0.010f));

    p.push_back (std::make_unique<juce::AudioParameterFloat>(
        "BETA", "Beta",
        0.000f, 0.100f, 0.020f));
    p.push_back (std::make_unique<juce::AudioParameterFloat>(
        "F11", "F11",
        50.0f, 1000.0f, 220.0f));

    p.push_back (std::make_unique<juce::AudioParameterFloat>(
        "BRX", "BridgeX",
        0.0f, 1.0f, 0.8f));

    p.push_back (std::make_unique<juce::AudioParameterFloat>(
        "BRY", "BridgeY",
        0.0f, 1.0f, 0.2f));

    p.push_back (std::make_unique<juce::AudioParameterFloat>(
        "Q", "Q",
        juce::NormalisableRange<float> (2.0f, 200.0f, 1.0f), 40.0f));

    p.push_back (std::make_unique<juce::AudioParameterFloat>(
        "MIX", "Mix",
        0.0f, 1.0f, 0.5f));

    p.push_back (std::make_unique<juce::AudioParameterFloat>(
        "MAXMODES", "MaxModes",
        juce::NormalisableRange<float> (1.0f, 144.0f, 1.0f), 16.0f));

    p.push_back (std::make_unique<juce::AudioParameterFloat>(
        "INGAIN", "InputGain",
        -24.0f, 24.0f, 0.0f));

    p.push_back (std::make_unique<juce::AudioParameterFloat>(
        "OUTGAIN", "OutputGain",
        -24.0f, 24.0f, 0.0f));

    return { p.begin(), p.end() };
}