#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "dsp/Engine.h"

namespace pid
{
inline juce::String band (int b, const char* p) { return "b" + juce::String (b + 1) + "_" + p; }
inline juce::String verb (int v, const char* p) { return juce::String (v == 0 ? "vs_" : "vl_") + p; }
}

struct Params
{
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout()
    {
        using namespace juce;
        AudioProcessorValueTreeState::ParameterLayout L;

        auto fl = [&] (const String& id, const String& name, NormalisableRange<float> r, float def, const String& unit = {})
        {
            auto toText = [unit] (float v, int) -> String
            {
                if (unit == "Hz") return v >= 1000.0f ? String (v / 1000.0f, 2) + " kHz" : String (roundToInt (v)) + " Hz";
                if (unit == "ms") return v < 10.0f ? String (v, 1) + " ms" : String (roundToInt (v)) + " ms";
                if (unit == "s")  return String (v, 2) + " s";
                if (unit == "dB") return (v > 0.04f ? "+" : "") + String (v, 1) + " dB";
                return String (v, 2);
            };
            auto fromText = [] (const String& t) -> float
            {
                const auto s = t.trim().toLowerCase();
                const float v = s.getFloatValue();
                return s.endsWith ("khz") || s.endsWith ("k") ? v * 1000.0f : v;
            };
            L.add (std::make_unique<AudioParameterFloat> (ParameterID { id, 1 }, name, r, def,
                       AudioParameterFloatAttributes().withLabel (unit)
                           .withStringFromValueFunction (toText).withValueFromStringFunction (fromText)));
        };
        auto skewed = [] (float lo, float hi, float centre)
        {
            NormalisableRange<float> r (lo, hi); r.setSkewForCentre (centre); return r;
        };
        auto onOff = [&] (const String& id, const String& name, bool def)
        {   // 2-step choice instead of AudioParameterBool: restores exactly in every host
            L.add (std::make_unique<AudioParameterChoice> (ParameterID { id, 1 }, name, StringArray { "Off", "On" }, def ? 1 : 0));
        };
        auto pct = [&] (const String& id, const String& name, float lo, float def)
        {
            L.add (std::make_unique<AudioParameterFloat> (ParameterID { id, 1 }, name, NormalisableRange<float> (lo, 100.0f, 0.1f), def,
                       AudioParameterFloatAttributes().withLabel ("%")
                           .withStringFromValueFunction ([] (float v, int) { return String (juce::roundToInt (v)) + "%"; })));
        };

        L.add (std::make_unique<AudioParameterChoice> (ParameterID { "bands", 1 }, "Bands", StringArray { "2", "3", "4" }, 1));
        fl ("x1", "Crossover 1", skewed (40.0f, 1000.0f, 150.0f), 150.0f, "Hz");
        fl ("x2", "Crossover 2", skewed (200.0f, 8000.0f, 1500.0f), 2500.0f, "Hz");
        fl ("x3", "Crossover 3", skewed (1500.0f, 16000.0f, 6000.0f), 8000.0f, "Hz");

        const float defAtk[4]  = { 25, 45, 30, 20 };
        const float defSus[4]  = { -15, -20, -10, -10 };
        const float defSend[4] = { 0, 100, 60, 40 };
        for (int b = 0; b < tb::kMaxBands; ++b)
        {
            const String n = "Band " + String (b + 1) + " ";
            pct (pid::band (b, "atk"),   n + "Attack",  -100.0f, defAtk[b]);
            pct (pid::band (b, "sus"),   n + "Sustain", -100.0f, defSus[b]);
            fl  (pid::band (b, "atime"), n + "Attack Time",  skewed (1.0f, 50.0f, 8.0f),     b == 0 ? 12.0f : 6.0f, "ms");
            fl  (pid::band (b, "stime"), n + "Sustain Time", skewed (20.0f, 1000.0f, 150.0f), 140.0f, "ms");
            pct (pid::band (b, "send"),  n + "Verb Send", 0.0f, defSend[b]);
            fl  (pid::band (b, "gain"),  n + "Gain", NormalisableRange<float> (-12.0f, 12.0f, 0.1f), 0.0f, "dB");
            onOff (pid::band (b, "solo"), n + "Solo", false);
            onOff (pid::band (b, "byp"),  n + "Bypass", false);
        }

        for (int v = 0; v < 2; ++v)
        {
            const bool s = v == 0;
            const String n = s ? "Short " : "Long ";
            pct (pid::verb (v, "level"),   n + "Level", 0.0f, s ? 35.0f : 22.0f);
            pct (pid::verb (v, "size"),    n + "Size",  0.0f, s ? 40.0f : 65.0f);
            fl  (pid::verb (v, "decay"),   n + "Decay", s ? skewed (0.1f, 2.5f, 0.6f) : skewed (0.5f, 12.0f, 3.0f), s ? 0.55f : 3.5f, "s");
            pct (pid::verb (v, "shimmer"), n + "Shimmer", 0.0f, s ? 25.0f : 55.0f);
            fl  (pid::verb (v, "tone"),    n + "Tone",    skewed (1000.0f, 18000.0f, 6000.0f), s ? 9000.0f : 7000.0f, "Hz");
            fl  (pid::verb (v, "lowcut"),  n + "Low Cut", skewed (20.0f, 2000.0f, 300.0f), s ? 350.0f : 500.0f, "Hz");
            fl  (pid::verb (v, "predelay"),n + "Pre-Delay", NormalisableRange<float> (0.0f, 150.0f, 0.1f), s ? 0.0f : 18.0f, "ms");
            pct (pid::verb (v, "width"),   n + "Width", 0.0f, 100.0f);
            fl  (pid::verb (v, "thresh"),  n + "Gate Threshold", NormalisableRange<float> (-60.0f, 0.0f, 0.1f), -34.0f, "dB");
            fl  (pid::verb (v, "hold"),    n + "Gate Hold",    s ? skewed (0.0f, 500.0f, 80.0f)  : skewed (0.0f, 2000.0f, 250.0f), s ? 60.0f : 160.0f, "ms");
            fl  (pid::verb (v, "release"), n + "Gate Release", s ? skewed (5.0f, 1000.0f, 120.0f) : skewed (10.0f, 4000.0f, 500.0f), s ? 90.0f : 450.0f, "ms");
            pct (pid::verb (v, "depth"),   n + "Gate Depth", 0.0f, s ? 100.0f : 70.0f);
        }

        pct ("mix", "Shaper Mix", 0.0f, 100.0f);
        fl  ("out", "Output", NormalisableRange<float> (-24.0f, 12.0f, 0.1f), 0.0f, "dB");
        onOff ("clip", "Soft Clip", true);
        return L;
    }

    // Cached raw pointers for fast, lock-free reads on the audio thread
    void attach (juce::AudioProcessorValueTreeState& s)
    {
        auto g = [&] (const juce::String& id) { auto* p = s.getRawParameterValue (id); jassert (p != nullptr); return p; };
        bands = g ("bands"); x[0] = g ("x1"); x[1] = g ("x2"); x[2] = g ("x3");
        for (int b = 0; b < tb::kMaxBands; ++b)
        {
            auto& B = band[b];
            B.atk = g (pid::band (b, "atk")); B.sus = g (pid::band (b, "sus"));
            B.atime = g (pid::band (b, "atime")); B.stime = g (pid::band (b, "stime"));
            B.send = g (pid::band (b, "send")); B.gain = g (pid::band (b, "gain"));
            B.solo = g (pid::band (b, "solo")); B.byp = g (pid::band (b, "byp"));
        }
        for (int v = 0; v < 2; ++v)
        {
            auto& V = verb[v];
            V.level = g (pid::verb (v, "level")); V.size = g (pid::verb (v, "size")); V.decay = g (pid::verb (v, "decay"));
            V.shimmer = g (pid::verb (v, "shimmer")); V.tone = g (pid::verb (v, "tone")); V.lowcut = g (pid::verb (v, "lowcut"));
            V.predelay = g (pid::verb (v, "predelay")); V.width = g (pid::verb (v, "width")); V.thresh = g (pid::verb (v, "thresh"));
            V.hold = g (pid::verb (v, "hold")); V.release = g (pid::verb (v, "release")); V.depth = g (pid::verb (v, "depth"));
        }
        mix = g ("mix"); out = g ("out"); clip = g ("clip");
    }

    tb::EngineParams read() const
    {
        tb::EngineParams p;
        p.numBands = 2 + (int) bands->load();
        for (int i = 0; i < 3; ++i) p.xover[i] = x[i]->load();
        for (int b = 0; b < tb::kMaxBands; ++b)
        {
            auto& B = band[b]; auto& o = p.band[b];
            o.attack = B.atk->load() * 0.01f; o.sustain = B.sus->load() * 0.01f;
            o.atkMs = B.atime->load(); o.susMs = B.stime->load();
            o.send = B.send->load() * 0.01f; o.gainDb = B.gain->load();
            o.solo = B.solo->load() > 0.5f; o.bypass = B.byp->load() > 0.5f;
        }
        for (int v = 0; v < 2; ++v)
        {
            auto& V = verb[v]; auto& o = p.verb[v];
            const float lv = V.level->load() * 0.01f;
            o.level = lv * lv;                       // perceptual taper
            o.size = V.size->load() * 0.01f; o.decay = V.decay->load();
            o.shimmer = V.shimmer->load() * 0.01f; o.toneHz = V.tone->load(); o.lowCutHz = V.lowcut->load();
            o.predelayMs = V.predelay->load(); o.width = V.width->load() * 0.01f; o.threshDb = V.thresh->load();
            o.holdMs = V.hold->load(); o.releaseMs = V.release->load(); o.gateDepth = V.depth->load() * 0.01f;
        }
        p.mix = mix->load() * 0.01f; p.outDb = out->load(); p.clip = clip->load() > 0.5f;
        return p;
    }

    using P = std::atomic<float>*;
    P bands {}, x[3] {}, mix {}, out {}, clip {};
    struct { P atk, sus, atime, stime, send, gain, solo, byp; } band[tb::kMaxBands] {};
    struct { P level, size, decay, shimmer, tone, lowcut, predelay, width, thresh, hold, release, depth; } verb[2] {};
};
