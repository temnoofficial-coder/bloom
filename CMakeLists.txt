cmake_minimum_required(VERSION 3.22)
project(TransientBloom VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>") # no VC++ redistributable needed

# Use a local JUCE checkout if given (-DJUCE_DIR=...), otherwise download it.
if(DEFINED JUCE_DIR)
    add_subdirectory(${JUCE_DIR} JUCE)
else()
    include(FetchContent)
    FetchContent_Declare(JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 8.0.4
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(JUCE)
endif()

option(TB_HEADLESS_LINUX "Local Linux test build without optional X11 extensions" OFF)
set(TB_FORMATS VST3 Standalone)
if(TB_HEADLESS_LINUX)
    set(TB_FORMATS VST3)
endif()

juce_add_plugin(TransientBloom
    COMPANY_NAME "Fin"
    PLUGIN_MANUFACTURER_CODE Tmno
    PLUGIN_CODE Tblm
    FORMATS ${TB_FORMATS}
    PRODUCT_NAME "Transient Bloom"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    VST3_CATEGORIES Fx Dynamics Reverb
    COPY_PLUGIN_AFTER_BUILD FALSE)

target_sources(TransientBloom PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp)

target_include_directories(TransientBloom PRIVATE Source)

target_compile_definitions(TransientBloom PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0)

if(TB_HEADLESS_LINUX)
    target_compile_definitions(TransientBloom PUBLIC
        JUCE_USE_XRANDR=0 JUCE_USE_XINERAMA=0 JUCE_USE_XCURSOR=0 JUCE_USE_XRENDER=0 JUCE_ALSA=0 JUCE_JACK=0)
endif()

target_link_libraries(TransientBloom
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# Dev-only: headless UI snapshot tool
if(TB_HEADLESS_LINUX)
    juce_add_console_app(TBSnapshot PRODUCT_NAME "TBSnapshot")
    target_sources(TBSnapshot PRIVATE tests/Snapshot.cpp Source/PluginProcessor.cpp Source/PluginEditor.cpp)
    target_include_directories(TBSnapshot PRIVATE Source)
    target_compile_definitions(TBSnapshot PRIVATE JucePlugin_Name="Transient Bloom" JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0
        JUCE_USE_XRANDR=0 JUCE_USE_XINERAMA=0 JUCE_USE_XCURSOR=0 JUCE_USE_XRENDER=0 JUCE_ALSA=0 JUCE_JACK=0)
    target_link_libraries(TBSnapshot PRIVATE juce::juce_audio_processors juce::juce_gui_extra juce::juce_recommended_config_flags)
endif()
