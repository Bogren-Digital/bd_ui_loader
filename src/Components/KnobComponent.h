#pragma once

class KnobComponent : public juce::Slider, public PlayfulTones::ComponentResizer, public OriginalSizeReporter
{
public:
    KnobComponent(const juce::String& name,juce::OwnedArray<juce::Image>& imagesToUse, UILoader::ComponentMetadata metadata, juce::Image maskImage)
    : juce::Slider(name)
    , PlayfulTones::ComponentResizer(*dynamic_cast<juce::Component*>(this))
    , OriginalSizeReporter(std::move(metadata))
    , resamplingMask(std::move(maskImage))
    , useGuiResampler(metadata.useGuiResampler)
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
            if (useGuiResampler)
            {
                const auto resampledImage = BogrenDigital::ImageResampler::applyResize(
                    *images[imageIndex], getWidth(), getHeight());
                g.drawImageAt(resampledImage, 0, 0);
            }
            else
            {
                g.drawImage(*images[imageIndex], getLocalBounds().toFloat());
            }
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
    juce::OwnedArray<juce::Image> images;
    juce::Image resamplingMask;
    const bool useGuiResampler = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobComponent)
};
