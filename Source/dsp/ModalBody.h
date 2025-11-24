#pragma once

#include <vector>
#include <cmath>
#include <JuceHeader.h>
#include "RectChladniPattern.h"

// Ein einzelner Modus des Resonators
struct ModalModeState
{
    int   modeIndex = 0;   // 1-basiert (1 = (1,1), 2 = (2,1), ...)
    int   m = 0;
    int   n = 0;

    float realTheta = 0.0f; // tatsächlicher Mischwinkel (nach Kopplung)

    float relFreq = 0.0f;   // NEU: relative Faktor zu f11
    float freqHz = 0.0f;    // abgeleitet: relFreq * f11
    float q      = 20.0f;

    // Kopplungs-Gain (inkl. u_k)
    float gain   = 1.0f;

    // IIR-Koeffizienten
    float a1 = 0.0f, a2 = 0.0f, b0 = 0.0f;

    // Filterzustände
    float y1 = 0.0f, y2 = 0.0f;

    // einfache Hüllkurve für GUI – normaler float, damit der Typ kopierbar bleibt
    float env = 0.0f;
};

class ModalBody
{
public:
    void setBaseFrequency (float f11)
    {
        baseF11 = f11;
        // update freqHz aus relFreq
        for (auto& m : modes)
            m.freqHz = m.relFreq * baseF11;

        updateCoeffs();
    }

    void setSampleRate (double sr)
    {
        sampleRate = (float) sr;
        updateCoeffs();
    }

    // Vollständige Modenliste setzen (z. B. in prepareToPlay gebaut)
    void setModes (std::vector<ModalModeState> newModes)
    {
        modes = std::move (newModes);
        updateCoeffs();
    }

    // Zugriff für GUI (nur lesen!)
    const std::vector<ModalModeState>& getModes() const noexcept
    {
        return modes;
    }

    // Pro Block: Kopplungs-Gains u_k anhand Bridge & theta aktualisieren
    void updateCouplings (float xBridge, float yBridge, float theta, float a, float b, float alpha, float beta)
    {
        const int N = getActiveModeCount();

        for (int i = 0; i < N; ++i)
        {
            auto& m = modes[(size_t) i];

            const auto mn = RectChladniPattern::modeIndexToMN (i + 1);

            m.realTheta = RectChladniPattern::computeRealTheta (
                mn.m, mn.n,
                theta,
                a, b, alpha, beta
            );

            float u = RectChladniPattern::computeU (m.modeIndex, m.realTheta, xBridge, yBridge);
            m.gain = u;
        }
    }

    // Ein Sample durch die gesamte Modenbank schicken
    float processSample (float x)
    {
        float sum = 0.0f;
        const float envAlpha = 0.001f;
        const int N = getActiveModeCount();

        for (int i = 0; i < N; ++i)
        {
            auto& m = modes[(size_t) i];

            const float y = m.a1 * m.y1 + m.a2 * m.y2 + m.b0 * (m.gain * x);

            m.y2 = m.y1;
            m.y1 = y;

            float newEnv = (1.0f - envAlpha) * m.env + envAlpha * std::abs (y);
            m.env = newEnv;

            sum += y;
        }

        return sum;
    }

    // Globalen Q-Wert für alle Moden setzen
    void setGlobalQ (float newQ)
    {
        for (auto& m : modes)
            m.q = newQ;

        updateCoeffs();
    }

    // Wie viele Moden aktiv benutzt werden (1..modes.size()).
    // n <= 0 oder n > size => alle Moden.
    void setActiveModeCount (int n)
    {
        const int oldN = getActiveModeCount();

        if (n <= 0 || n > (int) modes.size())
            activeModes = (int) modes.size();
        else
            activeModes = n;

        const int newN = getActiveModeCount();

        // falls wir reduziert haben: höhere Moden auf 0 setzen
        if (newN < oldN)
        {
            for (int i = newN; i < (int) modes.size(); ++i)
            {
                auto& m = modes[(size_t) i];
                m.y1 = m.y2 = 0.0f;
                m.env = 0.0f;
            }
        }
    }

    int getActiveModeCount() const noexcept
    {
        if (activeModes <= 0 || activeModes > (int) modes.size())
            return (int) modes.size();
        return activeModes;
    }    

private:
    void updateCoeffs()
    {
        if (sampleRate <= 0.0f)
            return;

        for (auto& m : modes)
        {
            if (m.freqHz <= 0.0f)
            {
                m.a1 = m.a2 = m.b0 = 0.0f;
                continue;
            }

            const float w = 2.0f * juce::MathConstants<float>::pi * m.freqHz / sampleRate;

            // einfacher Resonator mit Radius R
            const float R = std::exp (-juce::MathConstants<float>::pi * m.freqHz / (m.q * sampleRate));

            m.a1 = 2.0f * R * std::cos (w);
            m.a2 = -R * R;
            m.b0 = 1.0f - R; // grobe Anregungs-Skalierung
        }
    }

    float baseF11 = 220.0f;
    float sampleRate = 48000.0f;
    std::vector<ModalModeState> modes;

    int activeModes = -1; // -1 oder <=0 = alle Moden nutzen
};