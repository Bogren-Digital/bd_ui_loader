class CachedImageResampler : public juce::ComponentListener, private juce::Timer
{
public:
    CachedImageResampler(UILoader::ComponentMetadata metadata, juce::Component& wrappedComponent)
        : componentMetadata(std::move(metadata))
        , component(wrappedComponent)
    {
        component.addComponentListener(this);

        auto weakThis = juce::WeakReference<CachedImageResampler>(this);
        juce::MessageManager::getInstance()->callAsync([weakThis]()
        {
            if (auto* strongThis = weakThis.get())
            {
                strongThis->isEnabled = strongThis->componentMetadata.useGuiResampler;
                strongThis->handleResampling();
            }
        });
    }

    ~CachedImageResampler() override
    {
        component.removeComponentListener(this);
    }

    std::function<void()> onResampleImages = []
    {
        // Placeholder for the actual image resampling logic
        // This should be set by the owner of the CachedImageResampler object
        // it's the responsibility of the owner to populate the resampledImages array in this function
    };

    bool shouldDisplayResampledImages() const
    {
        return isResamplingDone && shouldBeResampling();
    }

    bool shouldBeResampling() const
    {
        return BogrenDigital::ImageResampler::shouldUseResampling(component.getLocalBounds(), componentMetadata.useGuiResampler);
    }

protected:
    UILoader::ComponentMetadata componentMetadata;
    juce::OwnedArray<juce::Image> resampledImages;
    std::atomic<bool> isResamplingDone = false;

private:
    juce::Component& component;
    std::atomic<bool> isEnabled = false;

    void handleResampling()
    {
        if (component.getWidth() <= 0 || component.getHeight() <= 0 || !isEnabled)
        {
            return;
        }
        
        if (shouldBeResampling())
        {
            const auto timeNowMs = juce::Time::getCurrentTime().toMilliseconds();
            isResamplingDone = false;
            resampledImages.clear();
            onResampleImages();
            isResamplingDone = true;
            component.repaint();
            auto timeTakenMs = juce::Time::getCurrentTime().toMilliseconds() - timeNowMs;
            juce::Logger::writeToLog("Resampling for " + component.getName() + " took " + juce::String(timeTakenMs) + " ms");
        }
    }

    void componentMovedOrResized(juce::Component& c, bool wasMoved, bool wasResized) override
    {
        juce::ignoreUnused(wasMoved);

        if (!wasResized)
        {
            return;
        }
        
        isResamplingDone = false;
        startTimer(500);
        // This will trigger a resampling after 500ms if the component is resized
    }

    void timerCallback() override
    {
        stopTimer();
        handleResampling();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CachedImageResampler)
    JUCE_DECLARE_WEAK_REFERENCEABLE(CachedImageResampler)
};