#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>
#include <utility>

// Factory presets: every preset starts from the parameter defaults, then applies these overrides.
struct Preset
{
    const char* name;
    std::vector<std::pair<const char*, float>> values; // id -> real (unnormalised) value
};

inline const std::vector<Preset>& getFactoryPresets()
{
    static const std::vector<Preset> presets = {
        { "DnB Default", {} },

        { "DnB Snare Crack", {
            { "bands", 1 }, { "x1", 180 }, { "x2", 3000 },
            { "b1_atk", 20 }, { "b1_sus", -10 },
            { "b2_atk", 70 }, { "b2_sus", -30 }, { "b2_atime", 4 },
            { "b3_atk", 50 }, { "b3_sus", -20 }, { "b3_atime", 3 },
            { "b1_send", 0 }, { "b2_send", 100 }, { "b3_send", 70 },
            { "vs_level", 45 }, { "vs_size", 30 }, { "vs_decay", 0.45f }, { "vs_shimmer", 30 }, { "vs_hold", 40 }, { "vs_release", 70 },
            { "vl_level", 15 } } },

        { "Tight Rollers", {
            { "bands", 1 }, { "x1", 140 }, { "x2", 2500 },
            { "b1_atk", 40 }, { "b2_atk", 35 }, { "b3_atk", 30 },
            { "b1_sus", -40 }, { "b2_sus", -35 }, { "b3_sus", -30 },
            { "b1_stime", 90 }, { "b2_stime", 90 }, { "b3_stime", 90 },
            { "vs_level", 25 }, { "vs_hold", 30 }, { "vs_release", 50 }, { "vl_level", 10 } } },

        { "Neuro Smash", {
            { "bands", 2 }, { "x1", 100 }, { "x2", 800 }, { "x3", 5000 },
            { "b1_atk", 50 }, { "b2_atk", 60 }, { "b3_atk", 55 }, { "b4_atk", 40 },
            { "b1_sus", -30 }, { "b2_sus", -10 }, { "b3_sus", -35 }, { "b4_sus", -40 },
            { "b1_atime", 10 }, { "b2_atime", 5 }, { "b3_atime", 3 }, { "b4_atime", 2 },
            { "b1_send", 0 }, { "b2_send", 40 }, { "b3_send", 100 }, { "b4_send", 50 },
            { "vs_level", 40 }, { "vs_shimmer", 15 }, { "vl_level", 0 }, { "clip", 1 }, { "out", -1 } } },

        { "Jungle Breaks Air", {
            { "bands", 2 }, { "x1", 120 }, { "x2", 1200 }, { "x3", 6000 },
            { "b1_atk", 30 }, { "b2_atk", 40 }, { "b3_atk", 45 }, { "b4_atk", 35 },
            { "b1_sus", 20 }, { "b2_sus", 10 }, { "b3_sus", 0 }, { "b4_sus", -10 },
            { "b1_send", 0 }, { "b2_send", 80 }, { "b3_send", 100 }, { "b4_send", 80 },
            { "vs_level", 25 },
            { "vl_level", 35 }, { "vl_decay", 5 }, { "vl_shimmer", 70 }, { "vl_depth", 40 }, { "vl_hold", 300 }, { "vl_release", 900 } } },

        { "Liquid Shimmer Snare", {
            { "bands", 1 },
            { "b1_atk", 15 }, { "b2_atk", 35 }, { "b3_atk", 25 },
            { "b1_sus", 0 }, { "b2_sus", 0 }, { "b3_sus", 10 },
            { "vs_level", 20 }, { "vs_shimmer", 40 },
            { "vl_level", 40 }, { "vl_shimmer", 80 }, { "vl_decay", 6 }, { "vl_depth", 30 }, { "vl_release", 1500 }, { "vl_tone", 9000 } } },

        { "Halftime Big Room", {
            { "bands", 1 },
            { "b1_atk", 45 }, { "b2_atk", 55 }, { "b3_atk", 30 },
            { "vs_level", 30 }, { "vs_size", 60 },
            { "vl_level", 40 }, { "vl_size", 90 }, { "vl_decay", 4 }, { "vl_hold", 400 }, { "vl_release", 600 }, { "vl_depth", 90 }, { "vl_shimmer", 35 } } },

        { "Kick Punch Only", {
            { "bands", 0 }, { "x1", 160 },
            { "b1_atk", 60 }, { "b1_sus", -25 }, { "b1_atime", 15 },
            { "b2_atk", 0 }, { "b2_sus", 0 },
            { "b1_send", 0 }, { "b2_send", 0 }, { "vs_level", 0 }, { "vl_level", 0 } } },

        { "Hip-Hop Knock", {
            { "bands", 1 },
            { "b1_atk", 35 }, { "b2_atk", 40 }, { "b3_atk", 15 },
            { "b1_sus", 10 }, { "b2_sus", -5 }, { "b3_sus", -20 },
            { "b1_atime", 15 }, { "b2_atime", 10 }, { "b3_atime", 8 },
            { "vs_level", 20 }, { "vs_shimmer", 0 }, { "vl_level", 0 } } },

        { "Rock Kit Room", {
            { "bands", 1 },
            { "b1_atk", 20 }, { "b2_atk", 30 }, { "b3_atk", 20 },
            { "b1_sus", 30 }, { "b2_sus", 30 }, { "b3_sus", 10 },
            { "b1_stime", 300 }, { "b2_stime", 300 }, { "b3_stime", 300 },
            { "vs_level", 20 }, { "vs_shimmer", 0 },
            { "vl_level", 30 }, { "vl_decay", 2.5f }, { "vl_shimmer", 0 }, { "vl_depth", 0 } } },

        { "Gentle Glue", {
            { "bands", 1 },
            { "b1_atk", -20 }, { "b2_atk", -15 }, { "b3_atk", -25 },
            { "b1_sus", 10 }, { "b2_sus", 10 }, { "b3_sus", 10 },
            { "vs_level", 0 }, { "vl_level", 0 }, { "mix", 60 } } },

        { "Pluck / Guitar Bloom", {
            { "bands", 1 }, { "x1", 250 }, { "x2", 3000 },
            { "b1_atk", 30 }, { "b2_atk", 30 }, { "b3_atk", 30 },
            { "b1_sus", -10 }, { "b2_sus", -10 }, { "b3_sus", -10 },
            { "b1_send", 50 }, { "b2_send", 100 }, { "b3_send", 100 },
            { "vs_level", 25 },
            { "vl_level", 35 }, { "vl_shimmer", 80 }, { "vl_decay", 7 }, { "vl_depth", 50 } } },
    };
    return presets;
}

inline void applyPreset (juce::AudioProcessorValueTreeState& state, int index)
{
    const auto& presets = getFactoryPresets();
    if (index < 0 || index >= (int) presets.size()) return;

    for (auto* p : state.processor.getParameters())
        if (auto* rp = dynamic_cast<juce::RangedAudioParameter*> (p))
            rp->setValueNotifyingHost (rp->getDefaultValue());

    for (auto& [id, value] : presets[(size_t) index].values)
        if (auto* rp = state.getParameter (id))
            rp->setValueNotifyingHost (rp->convertTo0to1 (value));
}
