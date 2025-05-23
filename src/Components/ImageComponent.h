#pragma once

class ImageComponent : public juce::Component, public PlayfulTones::ComponentResizer, public OriginalSizeReporter, public BogrenDigital::ImageResampler::DeferredImageResampler
{
public:
    ImageComponent(const juce::String& name, const juce::Image& imageToUse, UILoader::ComponentMetadata metadata, juce::Image maskImage = {})
    : juce::Component(name)
    , PlayfulTones::ComponentResizer(*dynamic_cast<juce::Component*>(this))
    , OriginalSizeReporter(std::move(metadata))
    , DeferredImageResampler(metadata.useGuiResampler, *dynamic_cast<juce::Component*>(this), std::move(maskImage))
    {
        setOpaque(false);
        images.add(new juce::Image(imageToUse));
    }
    
    void paint(juce::Graphics& g) override
    {
        if (images.size() > 0)
        {
            drawImage(g, 0);
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
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImageComponent)
};
