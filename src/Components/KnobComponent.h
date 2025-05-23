#pragma once

class KnobComponent : public juce::Slider, public PlayfulTones::ComponentResizer, public OriginalSizeReporter, public CachedImageResampler
{
public:
    KnobComponent(const juce::String& name,juce::OwnedArray<juce::Image>& imagesToUse, UILoader::ComponentMetadata metadata, juce::Image maskImage)
    : juce::Slider(name)
    , PlayfulTones::ComponentResizer(*dynamic_cast<juce::Component*>(this))
    , OriginalSizeReporter(std::move(metadata))
    , CachedImageResampler(metadata.useGuiResampler, *dynamic_cast<juce::Component*>(this), std::move(maskImage))
    {
        images.swapWith(imagesToUse); // Transfer ownership of images
        setSliderStyle(juce::Slider::RotaryVerticalDrag);
        setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        setOpaque(false);
    }
    
    void paint(juce::Graphics& g) override
    {
        // Draw the knob using the images
        if (images.size() > 0)
        {
            const auto normalizedValue = (getValue() - getMinimum()) / (getMaximum() - getMinimum());
            const auto imageIndex = static_cast<int>(normalizedValue * (images.size() - 1));
            drawImage(g, imageIndex);
        }
        else
        {
            // Fallback if no images are provided
            g.fillAll(juce::Colours::darkgrey.withAlpha(0.3f));
            g.setColour(juce::Colours::white);
            g.drawText("No Images Provided", getLocalBounds(), 
                       juce::Justification::centred, true);
        }
    }

private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobComponent)
};
