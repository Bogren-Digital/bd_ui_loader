class CachedImageResampler : public juce::ComponentListener, private juce::Timer
{
public:
    CachedImageResampler(bool shouldDoResampling, juce::Component& wrappedComponent, juce::Image maskImage = {})
        : resamplingMask(std::move(maskImage))
        , component(wrappedComponent)
    {
        component.addComponentListener(this);

        auto weakThis = juce::WeakReference<CachedImageResampler>(this);
        juce::MessageManager::getInstance()->callAsync([weakThis, shouldDoResampling]()
        {
            if (auto* strongThis = weakThis.get())
            {
                strongThis->isEnabled = shouldDoResampling;
                strongThis->handleResampling();
            }
        });
    }

    ~CachedImageResampler() override
    {
        component.removeComponentListener(this);
    }

    // This should be set by the owner of the CachedImageResampler object
    // it's the responsibility of the owner to populate the resampledImages array in this function
    std::function<void()> onResampleImages = [this]
    {
        for (auto* image : images)
        {
            auto resampledImage = BogrenDigital::ImageResampler::applyResize(
                    *image, resamplingMask, component.getWidth(), component.getHeight());
            resampledImages.add(new juce::Image(resampledImage));
        }
    };

    bool shouldDisplayResampledImages() const
    {
        return isResamplingDone && shouldBeResampling();
    }

    bool shouldBeResampling() const
    {
        return BogrenDigital::ImageResampler::shouldUseResampling(component.getLocalBounds(), isEnabled);
    }

    void drawImage(juce::Graphics& g, int imageIndex)
    {
        if (imageIndex < 0 || imageIndex >= images.size())
        {
            g.fillAll(juce::Colours::darkgrey.withAlpha(0.3f));
            g.setColour(juce::Colours::white);
            g.drawText("Invalid Image Index", component.getLocalBounds(), 
                       juce::Justification::centred, true);
            return;
        }

        if(shouldDisplayResampledImages())
        {
            const auto resampledImage = resampledImages[imageIndex];
            if (resampledImage == nullptr || !resampledImage->isValid())
            {
                g.fillAll(juce::Colours::darkgrey.withAlpha(0.3f));
                g.setColour(juce::Colours::white);
                g.drawText("Resampling Failed", component.getLocalBounds(), 
                            juce::Justification::centred, true);
                return;
            }
            g.drawImageAt(*resampledImage, 0, 0);
        }
        else
        {
            g.drawImage(*images[imageIndex], component.getLocalBounds().toFloat());
        }
    }

protected:
    juce::OwnedArray<juce::Image> images;
    juce::OwnedArray<juce::Image> resampledImages;
    juce::Image resamplingMask = {};
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
        juce::ignoreUnused(c, wasMoved);

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