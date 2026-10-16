#pragma once

#include <JuceHeader.h>
#include <vector>
#include <cmath>

// Rechteckiges Chladni-Muster:
// Basis: W_mn(x,y) = sin(m pi x) * sin(n pi y), x,y in [0,1].
class RectChladniPattern
{
public:
    // ---------- Reine Basis-Mode W(m,n,x,y) ----------
    static float W (int m, int n, float x, float y)
    {
        const float xx = juce::jlimit (0.0f, 1.0f, x);
        const float yy = juce::jlimit (0.0f, 1.0f, y);

        const float pi = juce::MathConstants<float>::pi;

        return std::sin (m * pi * xx)
             * std::sin (n * pi * yy);
    }

    struct MN
    {
        int m;
        int n;
    };

    // Mode-Index (1-basiert) -> (m,n) nach Dreieckszahl-Logik
    static MN modeIndexToMN (int modeIndex)
    {
        jassert (modeIndex >= 1);

        const double kd     = (double) modeIndex;
        const double mFloat = (-1.0 + std::sqrt (1.0 + 8.0 * kd)) * 0.5;
        const int    m      = (int) std::ceil (mFloat);

        const int Tm1 = (m - 1) * m / 2;  // T_{m-1}
        const int n   = modeIndex - Tm1;  // 1..m

        return { m, n };
    }

    // theta steuert Mischung von W_mn und W_nm:
    // a = cos(theta), b = sin(theta)
    // W_theta = a * W_mn + b * W_nm
    //
    // Für m == n sind W_mn und W_nm identisch → wir fallen einfach auf die Basis zurück.
    static float W_mixedAngle (int m, int n, float x, float y, float theta)
    {
        if (m == n)
            return W (m, n, x, y);

        const float a = std::cos (theta);
        const float b = std::sin (theta);

        const float Wmn = W (m, n, x, y);
        const float Wnm = W (n, m, x, y);

        return a * Wmn + b * Wnm;
    }

    static float computeU (int modeIndex,
                        float theta,
                        float xBridge,
                        float yBridge)
    {
        auto mn = modeIndexToMN (modeIndex);
        const int m = mn.m;
        const int n = mn.n;

        return W_mixedAngle (m, n, xBridge, yBridge, theta);
    }

    // relative Frequenz (f_11 = 1.0)
    static float relativeFreqFromMN (int m, int n)
    {
        const float mm = (float) (m * m);
        const float nn = (float) (n * n);
        return std::sqrt ((mm + nn) / 2.0f);   // /sqrt(2)^2. ///// HIER NOCH EINEN SWITCH EINBAUEN FÜR PLATTE VS. MEMBRAN!?
    }

    static float relativeFreqFromModeIndex (int modeIndex)
    {
        auto mn = modeIndexToMN (modeIndex);
        return relativeFreqFromMN (mn.m, mn.n);
    }

    static float computeRealTheta (int m, int n, float theta0, float A, float B, float alpha, float beta)
    {
        const float dm = float(m - n);
        const float mn = float(m * m + n * n);

        // Mode-abhängige Kopplungsverstärkung: fällt ab mit (m-n)^2 UND (m^2+n^2)
        const float g_mn =
            1.0f / (1.0f + beta  * dm * dm) *
            1.0f / (1.0f + alpha * mn);

        // eigentlicher Mischwinkel
        const float dtheta = 0.5f * std::atan2( B * g_mn, A );

        // globaler Holzfaserwinkel + modeabhängiges Delta
        return theta0 + dtheta;
    }

    // ---------- Chladni-Image der gemischten Mode (mit Winkel) ----------
    static juce::Image makeImageMixedAngle (int modeIndex,
                                            float theta,
                                            bool absolute = false,
                                            int resolution = 256)
    {
        juce::Image img (juce::Image::RGB, resolution, resolution, false);

        const auto mn = modeIndexToMN (modeIndex);
        const int m = mn.m;
        const int n = mn.n;

        for (int y = 0; y < resolution; ++y)
        {
            const float yy = (float) y / (float) (resolution - 1);

            for (int x = 0; x < resolution; ++x)
            {
                const float xx = (float) x / (float) (resolution - 1);

                float v = W_mixedAngle (m, n, xx, yy, theta);

                float norm = absolute ? std::abs (v)
                                      : 0.5f * (v + 1.0f);

                norm = juce::jlimit (0.0f, 1.0f, norm);

                const auto c = (juce::uint8) (norm * 255.0f);
                img.setPixelAt (x, y, juce::Colour (c, c, c));
            }
        }

        return img;
    }
};