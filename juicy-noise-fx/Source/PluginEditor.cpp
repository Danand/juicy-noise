/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JuicynoisefxAudioProcessorEditor::JuicynoisefxAudioProcessorEditor(JuicynoisefxAudioProcessor& processor)
    : AudioProcessorEditor(&processor),
      audioProcessor(processor)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
}

JuicynoisefxAudioProcessorEditor::~JuicynoisefxAudioProcessorEditor()
{
}

//==============================================================================
void JuicynoisefxAudioProcessorEditor::paint(juce::Graphics& graphics)
{
}

void JuicynoisefxAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
