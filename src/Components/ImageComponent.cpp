namespace BogrenDigital::UILoading
{

    ImageComponent::ImageComponent (const juce::String& name, const juce::Image& imageToUse, UILoader::ComponentMetadata metadata, juce::Image maskImage, juce::Image hitboxMaskImage)
        : juce::Component (name), DeferredImageResampler (*dynamic_cast<juce::Component*> (this), std::move (maskImage)), hitboxMask (std::move (hitboxMaskImage))
    {
        setOpaque (false);
        images.add (new juce::Image (imageToUse));
    }

    void ImageComponent::paint (juce::Graphics& g)
    {
        if (images.size() > 0)
        {
            drawImage (g, 0);
        }
        else
        {
            g.fillAll (juce::Colours::darkgrey.withAlpha (0.3f));
            g.setColour (juce::Colours::white);
            g.drawText ("Invalid Image: " + getName(), getLocalBounds(), juce::Justification::centred, true);
        }
    }

    bool ImageComponent::hitTest (int x, int y)
    {
        return HitBoxMaskTester::hitTest (*this, x, y, hitboxMask);
    }

} // namespace BogrenDigital::UILoading
