cmake_minimum_required(VERSION 3.15)
project(ChladniResonator VERSION 0.0.1)

set(JUCE_DIR "/Users/leonbartosch/JUCE")
add_subdirectory(${JUCE_DIR} juce)

juce_add_plugin(ChladniResonator
    COMPANY_NAME "TimbralModelling"
    BUNDLE_ID com.leonbartosch.chladniresonator
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    VST3_CATEGORY "Instrument"
    FORMATS VST3 Standalone
    PRODUCT_NAME "Chladni Resonator"
)

# >>> HINZUFÜGEN:
juce_generate_juce_header(ChladniResonator)
target_include_directories(ChladniResonator PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
# <<<

target_include_directories(ChladniResonator PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_sources(ChladniResonator PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/dsp/RectChladniPattern.h
)

target_link_libraries(ChladniResonator PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_devices
    juce::juce_audio_basics
    juce::juce_gui_basics
    juce::juce_dsp
)

target_compile_definitions(ChladniResonator PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)