#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Presets.h"

TransientBloomProcessor::TransientBloomProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      state (*this, nullptr, "TransientBloom", Params::createLayout())
{
    params.attach (state);
}

void TransientBloomProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    tmp.setSize (1, juce::jmax (1, samplesPerBlock));
    engine.prepare (sampleRate);
}

bool TransientBloomProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto out = layouts.getMainOutputChannelSet();
    if (out != juce::AudioChannelSet::mono() && out != juce::AudioChannelSet::stereo())
        return false;
    return layouts.getMainInputChannelSet() == out;
}

void TransientBloomProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int n = buffer.getNumSamples();
    const int numIn = getTotalNumInputChannels();
    for (int ch = numIn; ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear (ch, 0, n);

    const auto p = params.read();
    if (buffer.getNumChannels() >= 2)
    {
        engine.process (buffer.getWritePointer (0), buffer.getWritePointer (1), n, p);
    }
    else
    {
        // mono: run a stereo copy and fold back down
        tmp.setSize (1, n, false, false, true);
        tmp.copyFrom (0, 0, buffer, 0, 0, n);
        auto* l = buffer.getWritePointer (0);
        auto* r = tmp.getWritePointer (0);
        engine.process (l, r, n, p);
        for (int i = 0; i < n; ++i) l[i] = 0.5f * (l[i] + r[i]);
    }
}

juce::AudioProcessorEditor* TransientBloomProcessor::createEditor()
{
    return new TransientBloomEditor (*this);
}

int TransientBloomProcessor::getNumPrograms() { return (int) getFactoryPresets().size(); }

void TransientBloomProcessor::setCurrentProgram (int index)
{
    if (index < 0 || index >= getNumPrograms()) return;
    currentProgram = index;
    applyPreset (state, index);
}

const juce::String TransientBloomProcessor::getProgramName (int index)
{
    if (index < 0 || index >= getNumPrograms()) return {};
    return getFactoryPresets()[(size_t) index].name;
}

void TransientBloomProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto s = state.copyState();
    s.setProperty ("program", currentProgram, nullptr);
    if (auto xml = s.createXml())
        copyXmlToBinary (*xml, destData);
}

void TransientBloomProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (state.state.getType()))
        {
            auto s = juce::ValueTree::fromXml (*xml);
            currentProgram = s.getProperty ("program", 0);
            state.replaceState (s);
        }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TransientBloomProcessor();
}
