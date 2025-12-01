# bd_ui_loader

A JUCE module for dynamically loading and managing UI components from XML metadata. Provides automatic layout management, aspect-ratio-preserving resizing, and an extensible component factory system.

- [bd\_ui\_loader](#bd_ui_loader)
  - [Overview](#overview)
  - [Features](#features)
  - [Dependencies](#dependencies)
  - [Integration](#integration)
    - [1. Basic Setup](#1-basic-setup)
    - [2. Accessing Components by Name](#2-accessing-components-by-name)
    - [3. Custom Resizing Behavior](#3-custom-resizing-behavior)
  - [XML Metadata Format](#xml-metadata-format)
    - [Supported Component Types](#supported-component-types)
  - [Creating Custom Components](#creating-custom-components)
    - [1. Define Your Component](#1-define-your-component)
    - [2. Create a Component Factory](#2-create-a-component-factory)
    - [3. Register Your Factory](#3-register-your-factory)
    - [4. Advanced Factory Registration](#4-advanced-factory-registration)
  - [Component Metadata](#component-metadata)
  - [Best Practices](#best-practices)
  - [Example: Complete Plugin Editor](#example-complete-plugin-editor)


## Overview

`bd_ui_loader` simplifies UI development by separating component layout from code. Design your interface, export component metadata to XML, and let the UILoader handle component creation, positioning, and resizing automatically.

## Features

- **XML-driven UI loading** - Define your UI structure in XML metadata
- **Automatic layout management** - Components are positioned and sized based on metadata
- **Aspect ratio preservation** - Parent component maintains bitmap aspect ratio during resizing
- **Component factory system** - Extensible architecture for custom component types
- **Binary asset integration** - Load images directly from JUCE's BinaryData
- **Built-in component types** - Knobs, switches, buttons, images, combo boxes, and more

## Dependencies

- `juce_gui_basics` - Core JUCE GUI functionality
- `playfultones_smoothresizing` - Smooth resizing utilities
- `bd_image_resampler` - High-quality image resampling
- `bd_binary_asset_utilities` - Binary asset loading utilities

This library includes [BS::thread_pool](https://github.com/bshoshany/thread-pool) by Barak Shoshany, licensed under the MIT License.

## Integration

### 1. Basic Setup

```cpp
#include <bd_ui_loader/bd_ui_loader.h>

class MyPluginEditor : public juce::AudioProcessorEditor
{
public:
    MyPluginEditor(MyAudioProcessor& processor)
        : AudioProcessorEditor(processor)
        , imageLoader(BinaryData::namedResourceList,
                      BinaryData::namedResourceListSize,
                      BinaryData::getNamedResource,
                      BinaryData::originalFilenames)
    {
        addAndMakeVisible(uiContainer);

        // UILoader takes a container component where UI will be loaded
        uiLoader = std::make_unique<BogrenDigital::UILoading::UILoader>(uiContainer, imageLoader);

        // Load UI from XML metadata
        uiLoader->loadUI("ui_metadata.xml");

        // UILoader automatically sets the container size based on bitmap dimensions
        // and maintains aspect ratio
        setSize(uiContainer.getWidth(), uiContainer.getHeight());
    }

    void resized() override
    {
        // Size the container first, then apply layout
        uiContainer.setBounds(getLocalBounds());
        uiLoader->applyLayout();
    }

private:
    juce::Component uiContainer;
    BogrenDigital::UILoading::BinaryAssetImageLoader imageLoader;
    std::unique_ptr<BogrenDigital::UILoading::UILoader> uiLoader;
};
```

### 2. Accessing Components by Name

```cpp
// Get a component by name from the loaded UI
if (auto* knob = uiLoader->getComponentByName("gain_knob"))
{
    // Cast to specific type and configure
    if (auto* knobComponent = dynamic_cast<BogrenDigital::UILoading::KnobComponent*>(knob))
    {
        // Attach parameter, set listeners, etc.
        knobComponent->onValueChange = [this](double value) {
            // Handle value changes
        };
    }
}

// Iterate through all loaded components
for (const auto& [name, component] : uiLoader->getComponentsByName())
{
    juce::Logger::writeToLog("Loaded component: " + name);
}
```

### 3. Custom Resizing Behavior

```cpp
class MyPluginEditor : public juce::AudioProcessorEditor
{
public:
    MyPluginEditor(MyAudioProcessor& processor)
        : AudioProcessorEditor(processor)
        , imageLoader(/* ... */)
    {
        addAndMakeVisible(uiContainer);
        uiLoader = std::make_unique<BogrenDigital::UILoading::UILoader>(uiContainer, imageLoader);
        uiLoader->loadUI("ui_metadata.xml");

        // Make the editor resizable while maintaining aspect ratio
        setResizable(true, true);
        setSize(uiContainer.getWidth(), uiContainer.getHeight());
    }

    void resized() override
    {
        // Size the container first, then apply layout
        // UILoader automatically maintains aspect ratio and repositions components
        uiContainer.setBounds(getLocalBounds());
        uiLoader->applyLayout();
    }

private:
    juce::Component uiContainer;
    BogrenDigital::UILoading::BinaryAssetImageLoader imageLoader;
    std::unique_ptr<BogrenDigital::UILoading::UILoader> uiLoader;
};
```

## XML Metadata Format

The UILoader expects XML in the following format:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<UI width="800" height="600">
    <!-- Image component -->
    <IMAGE name="background" file="Background.png"
           x="0" y="0" width="800" height="600"
           imageType="raster"/>

    <!-- Knob component -->
    <KNOB name="gain_knob" file="Knob.png"
          x="100" y="100" width="80" height="80"
          numberOfFrames="101" imageType="raster"/>

    <!-- Switch component -->
    <SWITCH name="power_switch" file="Switch.png"
            x="300" y="100" width="50" height="100"
            numberOfFrames="2" imageType="raster"/>

    <!-- Radio button group -->
    <BUTTONS name="mode_selector"
             fileNamePrefix="Mode_" fileNameSuffix=".png"
             x="400" y="100" width="200" height="50"
             numberOfFrames="3" imageType="raster"/>

    <!-- Combo box / dropdown -->
    <DROPDOWN name="preset_selector"
              x="500" y="200" width="150" height="30"/>

    <!-- Tweenable component (animated position) -->
    <TWEENABLE name="led_indicator" file="LED.png"
               minX="100" minY="50" maxX="200" maxY="50"
               width="20" height="20" imageType="raster"/>
</UI>
```

### Supported Component Types

| Type | Description | Key Attributes |
|------|-------------|----------------|
| `IMAGE` | Static image display | `file`, `imageType` |
| `KNOB` / `DIAL` | Rotary knob control | `file`, `numberOfFrames` |
| `SWITCH` | Toggle switch | `file`, `numberOfFrames` (typically 2) |
| `BUTTONS` | Radio button group | `fileNamePrefix`, `fileNameSuffix`, `numberOfFrames` |
| `DROPDOWN` | Combo box selector | Standard position/size attributes |
| `TWEENABLE` | Position-animated component | `minX`, `minY`, `maxX`, `maxY` |
| `HOOVERABLE` / `LED` | Hover-responsive component | `file`, `numberOfFrames` |

## Creating Custom Components

### 1. Define Your Component

```cpp
namespace BogrenDigital::UILoading
{
    class MyCustomComponent : public juce::Component
    {
    public:
        MyCustomComponent(const juce::String& name, const UILoader::ComponentMetadata& metadata)
            : componentName(name)
        {
            // Initialize component based on metadata
            setName(name);
        }

        void paint(juce::Graphics& g) override
        {
            // Custom painting logic
        }

    private:
        juce::String componentName;
    };
}
```

### 2. Create a Component Factory

```cpp
namespace BogrenDigital::UILoading
{
    class MyCustomComponentFactory : public ComponentFactory
    {
    public:
        MyCustomComponentFactory(BinaryAssetImageLoader& imgLoader)
            : ComponentFactory(imgLoader)
        {
        }

        juce::Component* createComponent(const UILoader::ComponentMetadata& metadata) override
        {
            return new MyCustomComponent(metadata.name, metadata);
        }
    };
}
```

### 3. Register Your Factory

```cpp
// In your UILoader setup (before loadUI)
uiLoader->getComponentFactoryRegistry().registerFactory<MyCustomComponentFactory>(
    uiLoader.get(),
    "MYCUSTOMTYPE",  // XML tag name
    "raster"         // Optional: imageType discriminator
);

// Now you can use <MYCUSTOMTYPE> in your XML metadata
```

### 4. Advanced Factory Registration

```cpp
// Register with additional constructor arguments
uiLoader->getComponentFactoryRegistry().registerFactory<MyAdvancedFactory>(
    uiLoader.get(),
    "ADVANCED",
    "raster",
    std::ref(someReference),  // Use std::ref() for reference parameters
    42,                        // Additional constructor arguments
    "config_value"
);
```

## Component Metadata

All components have access to their metadata through JUCE's component properties:

```cpp
auto* component = uiLoader->getComponentByName("my_component");
auto& props = component->getProperties();

// Access metadata fields
juce::String name = props["name"].toString();
int x = props["x"];
int y = props["y"];
int width = props["width"];
int height = props["height"];
juce::String imageType = props["imageType"].toString();
```

Available metadata fields:
- `type` - Component type (IMAGE, KNOB, SWITCH, etc.)
- `name` - Unique component identifier
- `file` - Image filename
- `fileNamePrefix` / `fileNameSuffix` - For multi-file components
- `imageType` - Image type discriminator (raster, vector)
- `x`, `y`, `width`, `height` - Component bounds
- `numberOfFrames` - For filmstrip images
- `minX`, `minY`, `maxX`, `maxY` - For tweenable components

## Best Practices

1. **Unique component names** - Ensure all components have unique names in your XML
2. **Aspect ratio awareness** - Design your UI with a fixed aspect ratio in mind
3. **Binary data organization** - Keep your binary resources organized with clear naming
4. **Factory registration** - Register all custom factories before calling `loadUI()`
5. **Component lifecycle** - UILoader manages component ownership; don't delete components manually

## Example: Complete Plugin Editor

```cpp
class FullPluginEditor : public juce::AudioProcessorEditor
{
public:
    FullPluginEditor(MyAudioProcessor& p)
        : AudioProcessorEditor(p)
        , processor(p)
        , imageLoader(BinaryData::namedResourceList,
                      BinaryData::namedResourceListSize,
                      BinaryData::getNamedResource,
                      BinaryData::originalFilenames)
    {
        addAndMakeVisible(uiContainer);

        // Create UILoader with container component
        uiLoader = std::make_unique<BogrenDigital::UILoading::UILoader>(uiContainer, imageLoader);

        // Load the UI
        uiLoader->loadUI("my_plugin_ui.xml");

        // Connect controls to parameters
        connectControlsToParameters();

        // Make resizable
        setResizable(true, true);
        setResizeLimits(400, 300, 1600, 1200);
        setSize(uiContainer.getWidth(), uiContainer.getHeight());
    }

    void resized() override
    {
        // Size the container first, then apply layout
        uiContainer.setBounds(getLocalBounds());
        uiLoader->applyLayout();
    }

private:
    void connectControlsToParameters()
    {
        // Connect gain knob
        if (auto* gainKnob = dynamic_cast<BogrenDigital::UILoading::KnobComponent*>(
                uiLoader->getComponentByName("gain_knob")))
        {
            gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                processor.getValueTreeState(), "gain", *gainKnob);
        }

        // Connect other controls...
    }

    MyAudioProcessor& processor;
    juce::Component uiContainer;
    BogrenDigital::UILoading::BinaryAssetImageLoader imageLoader;
    std::unique_ptr<BogrenDigital::UILoading::UILoader> uiLoader;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
};
```