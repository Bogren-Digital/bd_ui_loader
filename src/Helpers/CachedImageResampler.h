class CachedImageResampler : public juce::ComponentListener
{
public:
    CachedImageResampler(UILoader::ComponentMetadata metadata, juce::Component& wrappedComponent)
        : componentMetadata(std::move(metadata))
        , component(wrappedComponent)
    {
        component.addComponentListener(this);

        juce::MessageManager::getInstance()->callAsync([this]()
        {
            isEnabled = componentMetadata.useGuiResampler;
            handleResampling();
        });
    }

    ~CachedImageResampler() override
    {
        component.removeComponentListener(this);
    }

    void componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized) override
    {
        if (!wasResized)
        {
            return;
        }
        
        handleResampling();
    }

    std::function<void()> onResampleImages = []
    {
        // Placeholder for the actual image resampling logic
        // This should be set by the owner of the CachedImageResampler object
        // it's the responsibility of the owner to populate the resampledImages array in this function
    };

protected:
    UILoader::ComponentMetadata componentMetadata;
    juce::OwnedArray<juce::Image> resampledImages;
    std::atomic<bool> isEnabled = false;

private:
    juce::Component& component;

    void handleResampling()
    {
        if (component.getWidth() <= 0 || component.getHeight() <= 0 || !isEnabled)
        {
            return;
        }
        
        if (BogrenDigital::ImageResampler::shouldUseResampling(component.getLocalBounds(), componentMetadata.useGuiResampler))
        {
            const auto timeNowMs = juce::Time::getCurrentTime().toMilliseconds();
            resampledImages.clear();
            onResampleImages();
            component.repaint();
            auto timeTakenMs = juce::Time::getCurrentTime().toMilliseconds() - timeNowMs;
            juce::Logger::writeToLog("Resampling for " + component.getName() + " took " + juce::String(timeTakenMs) + " ms");
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CachedImageResampler)
};