#pragma once

#include <JuceHeader.h>
#include <atomic>
#include <vector>
#include "dsp/ModalBody.h"

struct SimpleLimiter
{
    void setSampleRate (double sr)
    {
        sampleRate = (float) sr;
        updateReleaseCoeff();
    }

    void setThresholdDb (float db)
    {
        thresholdDb  = db;
        thresholdLin = juce::Decibels::decibelsToGain (thresholdDb);
    }

    void setReleaseMs (float ms)
    {
        releaseMs = ms;
        updateReleaseCoeff();
    }

    float processSample (float x)
    {
        const float in = x;
        const float absIn = std::abs (in);

        // Peak-Envelope
        if (absIn > env)
            env = absIn;
        else
            env = env * releaseCoeff + (1.0f - releaseCoeff) * absIn;

        float gain = 1.0f;

        if (env > thresholdLin && env > 0.0f)
            gain = thresholdLin / env;

        return in * gain;
    }

private:
    void updateReleaseCoeff()
    {
        if (sampleRate <= 0.0f)
        {
            releaseCoeff = 0.0f;
            return;
        }

        const float releaseSeconds = releaseMs * 0.001f;
        releaseCoeff = std::exp (-1.0f / (releaseSeconds * sampleRate));
    }

    float sampleRate   = 48000.0f;
    float thresholdDb  = -1.0f;
    float thresholdLin = juce::Decibels::decibelsToGain (-1.0f);

    float releaseMs    = 50.0f;    // 50 ms
    float releaseCoeff = 0.0f;

    float env          = 0.0f;
};

class ChladniProcessor : public juce::AudioProcessor
{
public:
    ChladniProcessor();
    ~ChladniProcessor() override;

    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    //==============================================================================    
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #if ! JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    bool hasEditor() const override;
    juce::AudioProcessorEditor* createEditor() override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    const std::vector<ModalModeState>& getModalModes() const noexcept
    {
        return modalBodies[0].getModes(); // linker Körper für Visualisierung
    }

    float getF11() const       { return pF11 ? pF11->load() : 0.0f; }
    float getThetaMix() const  { return pTheta ? pTheta->load() : 0.0f; }
    float getA() const         { return pA ? pA->load() : 0.0f; }
    float getB() const         { return pB ? pB->load() : 0.0f; }
    float getAlpha() const     { return pAlpha ? pAlpha->load() : 0.0f; }
    float getBeta() const      { return pBeta ? pBeta->load() : 0.0f; }
    float getBridgeX() const   { return pBridgeX ? pBridgeX->load() : 0.0f; }
    float getBridgeY() const   { return pBridgeY ? pBridgeY->load() : 0.0f; }
    float getQGlobal() const   { return pQ ? pQ->load() : 0.0f; }
    float getMixWet() const    { return pMix ? pMix->load() : 0.0f; }
    float getInGainDb() const  { return pInGain ? pInGain->load() : 0.0f; }
    float getOutGainDb() const { return pOutGain ? pOutGain->load() : 0.0f; }

    int getNumModes() const    { return pModes ? (int)pModes->load() : 0; }

private:
    ModalBody modalBodies[2];   // 0 = left, 1 = right
    SimpleLimiter limiters[2];

    std::atomic<float>* pF11     = nullptr;
    std::atomic<float>* pTheta   = nullptr;
    std::atomic<float>* pA   = nullptr;
    std::atomic<float>* pB   = nullptr;
    std::atomic<float>* pAlpha   = nullptr;
    std::atomic<float>* pBeta   = nullptr;
    std::atomic<float>* pBridgeX = nullptr;
    std::atomic<float>* pBridgeY = nullptr;
    std::atomic<float>* pQ       = nullptr;
    std::atomic<float>* pMix     = nullptr;
    std::atomic<float>* pModes   = nullptr;
    std::atomic<float>* pInGain  = nullptr;
    std::atomic<float>* pOutGain = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChladniProcessor)
};