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
        if (component.getWidth() <= 0 || component.getHeight() <= 0)
        {
            // If the component is not visible, do not resample
            return;
        }
        
        if (wasResized && BogrenDigital::ImageResampler::shouldUseResampling(component.getLocalBounds(), componentMetadata.useGuiResampler))
        {
            const auto timeNowMs = juce::Time::getCurrentTime().toMilliseconds();
            resampledImages.clear();
            resampleImages();
            // Trigger a repaint to update the component with the new images
            component.repaint();
            // Log the time taken for resampling
            auto timeTakenMs = juce::Time::getCurrentTime().toMilliseconds() - timeNowMs;
            juce::Logger::writeToLog("Resampling for " + component.getName() + " took " + juce::String(timeTakenMs) + " ms");
        }
    }

    std::function<void()> resampleImages = []
    {
        // Placeholder for the actual image resampling logic
        // This should be set by the owner of the CachedImageResampler object
        // it's the responsibility of the owner to populate the resampledImages array in this function
    };

protected:
    UILoader::ComponentMetadata componentMetadata;
    juce::OwnedArray<juce::Image> resampledImages;

private:
    juce::Component& component;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CachedImageResampler)
};