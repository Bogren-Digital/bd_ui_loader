class CachedImageResampler : public juce::ComponentListener
{
public:
    CachedImageResampler(UILoader::ComponentMetadata metadata, juce::Component& wrappedComponent)
        : componentMetadata(std::move(metadata))
        , component(wrappedComponent)
    {
        // Listen to resize events
        component.addComponentListener(this);
    }

    ~CachedImageResampler() override
    {
        // Stop listening to resize events
        component.removeComponentListener(this);
    }

    void componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized) override
    {
        if (wasResized && BogrenDigital::ImageResampler::shouldUseResampling(component.getLocalBounds(), componentMetadata.useGuiResampler))
        {
            resampledImages.clear();
            resampleImages();
            // Trigger a repaint to update the component with the new images
            component.repaint();
        }
    }

    std::function<void()> resampleImages = []
    {
        // Placeholder for the actual image resampling logic
        // This should be overridden in derived classes to perform the resampling
        // it's the responsibility of the derived class to populate the resampledImages array in this function
    };

protected:
    UILoader::ComponentMetadata componentMetadata;
    juce::OwnedArray<juce::Image> resampledImages;

private:
    juce::Component& component;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CachedImageResampler)
};