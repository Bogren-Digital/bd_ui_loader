#pragma once

class ImageComponent : public juce::Component, public PlayfulTones::ComponentResizer, public OriginalSizeReporter
{
public:
    ImageComponent(const juce::String& name, const juce::Image& imageToUse, UILoader::ComponentMetadata metadata)
    : juce::Component(name)
    , PlayfulTones::ComponentResizer(*dynamic_cast<juce::Component*>(this))
    , OriginalSizeReporter(std::move(metadata))
    , image(imageToUse)
    {
        setOpaque(false);
    }
    
    void paint(juce::Graphics& g) override
    {
        // Draw the image to fill the entire component bounds
        if (image.isValid())
        {
            g.drawImage(image, getLocalBounds().toFloat());
        }
        else
        {
            // Fallback if image is invalid
            g.fillAll(juce::Colours::darkgrey.withAlpha(0.3f));
            g.setColour(juce::Colours::white);
            g.drawText("Invalid Image: " + getName(), getLocalBounds(), 
                       juce::Justification::centred, true);
        }
    }

private:
    juce::Image image;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImageComponent)
};
