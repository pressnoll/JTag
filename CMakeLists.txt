cmake_minimum_required(VERSION 3.22)
project(JTag VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.0
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(JTag
    COMPANY_NAME "VocalPlugins"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE VcPl
    PLUGIN_CODE JTag
    FORMATS VST3 AU
    PRODUCT_NAME "JTag"
)

target_sources(JTag PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/Synthesis/VocalSynthEngine.cpp
    Source/Synthesis/VocalSynthEngine.h
    Source/Synthesis/PhonemeEngine.h
    Source/Synthesis/FormantTables.h
    Source/DSP/TimeStretcher.cpp
    Source/DSP/TimeStretcher.h
    Source/UI/WaveformDragComponent.cpp
    Source/UI/WaveformDragComponent.h
)

target_compile_definitions(JTag PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(JTag PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_dsp
)

juce_generate_juce_header(JTag)