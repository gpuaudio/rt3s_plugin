cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
cmake_policy(SET CMP0025 NEW)
cmake_policy(SET CMP0135 NEW)

project(GpuaSourceSeparation VERSION 0.0.1)

if (DEFINED FETCH_LOCATION)
    # RT3S Model
    FetchContent_Declare(
        rt3s_model
        URL https://github.com/gpuaudio/platform_headers/releases/download/v0.0.1/RT3S_model.zip
        SOURCE_DIR ${FETCH_LOCATION}/rt3s_model
    )
    FetchContent_MakeAvailable(rt3s_model)
endif()

# Set standard C++ version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -----------------------------------------------------------------------------
# JUCE Dependency (Submodule)
# -----------------------------------------------------------------------------
# This assumes you have initalized submodules
add_subdirectory(JUCE)
# -----------------------------------------------------------------------------
# Plugin Configuration
# -----------------------------------------------------------------------------
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND plugin_formats VST3)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    set(install_dir "$ENV{CommonProgramFiles}/VST3")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    list(APPEND plugin_formats AU)
    set(install_dir "$ENV{HOME}/Library/Audio/Plug-Ins/Components")
endif()

juce_add_plugin(GpuaSourceSeparation
    # Company Metadata
    COMPANY_NAME "GPUAUDIO"
    BUNDLE_IDENTIFIER "com.gpu.audio.GpuaSourceSeparation"
    PLUGIN_MANUFACTURER_CODE "GPUA"
    PLUGIN_CODE "SoSe"

    FORMATS ${plugin_formats}

    # Capabilities
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE

    # Plugin Name
    PRODUCT_NAME "gpuaudio_source_separation"
    DESCRIPTION "GPU AUDIO Source Separation"
)

# This macro generates the "JuceHeader.h" file and adds the correct include
# directories to the target. Without this, #include <JuceHeader.h> will fail.
juce_generate_juce_header(GpuaSourceSeparation)

include (${CMAKE_CURRENT_LIST_DIR}/debug_vars.cmake)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if (VST3_DEPLOY)
        add_custom_command(TARGET GpuaSourceSeparation POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy "${rt3s_model_SOURCE_DIR}/params.bw" "${CMAKE_BINARY_DIR}/SoundSourceSeparation/rt3s_plugin/GpuaSourceSeparation_artefacts/$<CONFIG>/VST3/gpuaudio_source_separation.vst3/Contents/Resources"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/SoundSourceSeparation/rt3s_plugin/GpuaSourceSeparation_artefacts/$<CONFIG>/VST3/gpuaudio_source_separation.vst3" "${install_dir}/gpuaudio_source_separation.vst3"
            COMMAND ${CMAKE_COMMAND} -E echo "VST3 copied to ${install_dir}"
        )
    endif()
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    if (AUv2_DEPLOY)
        add_custom_command(TARGET GpuaSourceSeparation_AU POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy "${rt3s_model_SOURCE_DIR}/params.bw" "${CMAKE_BINARY_DIR}/SoundSourceSeparation/rt3s_plugin/GpuaSourceSeparation_artefacts/$<CONFIG>/AU/gpuaudio_source_separation.component/Contents/Resources/params.bw"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/SoundSourceSeparation/rt3s_plugin/GpuaSourceSeparation_artefacts/$<CONFIG>/AU/gpuaudio_source_separation.component" "${install_dir}/gpuaudio_source_separation.component"
            COMMAND ${CMAKE_COMMAND} -E echo "AUv2 copied to ${install_dir}"
        )
    endif()
endif()

# -----------------------------------------------------------------------------
# Source Files
# -----------------------------------------------------------------------------
target_sources(GpuaSourceSeparation
    PRIVATE
        src/PluginProcessor.cpp
        src/PluginProcessor.h
)

# -----------------------------------------------------------------------------
# Link Libraries & Compile Definitions
# -----------------------------------------------------------------------------
target_compile_definitions(GpuaSourceSeparation
    PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL_ENABLED are often not needed for simple audio plugins
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_SILENCE_XCODE_15_LINKER_WARNING=1
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# Add DemucsLib/include to the search path
target_include_directories(GpuaSourceSeparation
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/SoundSourceSeparation/RT3SLib/include)

# Link against the necessary JUCE modules
target_link_libraries(GpuaSourceSeparation
    PRIVATE
        juce::juce_audio_utils
        rt3slib
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

target_compile_features(GpuaSourceSeparation PRIVATE cxx_std_20)
