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
    constexpr static auto delayTimeMs = 500;

private:
    juce::Component& component;
    std::atomic<bool> isEnabled = false;

    struct ImageResamplingThreadPool
    {
    public:
        ImageResamplingThreadPool() = default;
        ~ImageResamplingThreadPool()
        {
            threadPool.removeAllJobs(true, delayTimeMs);
        }
        juce::ThreadPool threadPool { juce::ThreadPoolOptions{}
            .withThreadName ("Image Resampling Thread")
            .withNumberOfThreads (1) };
    };

    juce::SharedResourcePointer<ImageResamplingThreadPool> threadPool;

    class ResamplingJob : public juce::ThreadPoolJob
    {
    public:
        ResamplingJob(CachedImageResampler& r)
            : juce::ThreadPoolJob("Image Resampling job"),
            resampler(r)
        {
        }

        ~ResamplingJob() override = default;

    private:
        juce::ThreadPoolJob::JobStatus runJob() override
        {
            if (shouldExit())
                return juce::ThreadPoolJob::jobHasFinished;

            const auto timeNowMs = juce::Time::getCurrentTime().toMilliseconds();
            resampler.isResamplingDone = false;
            resampler.resampledImages.clear();
            for (auto* image : resampler.images)
            {
                if (shouldExit())
                    return juce::ThreadPoolJob::jobHasFinished;
                
                auto resampledImage = BogrenDigital::ImageResampler::applyResize(
                        *image, resampler.resamplingMask, resampler.component.getWidth(), resampler.component.getHeight());
                resampler.resampledImages.add(new juce::Image(resampledImage));
            }
            resampler.isResamplingDone = true;
            auto weakThis = juce::WeakReference<CachedImageResampler>(&resampler);
            juce::MessageManager::getInstance()->callAsync([weakThis]()
            {
                if (auto* strongThis = weakThis.get())
                {
                    strongThis->component.repaint();
                }
            });
            auto timeTakenMs = juce::Time::getCurrentTime().toMilliseconds() - timeNowMs;
            juce::Logger::writeToLog("Resampling for " + resampler.component.getName() + " took " + juce::String(timeTakenMs) + " ms");
            
            return juce::ThreadPoolJob::jobHasFinished;
        }

        CachedImageResampler& resampler;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResamplingJob)
    };

    void handleResampling()
    {
        if (component.getWidth() <= 0 || component.getHeight() <= 0 || !isEnabled)
        {
            return;
        }
        
        if (shouldBeResampling())
        {
            threadPool->threadPool.addJob(new ResamplingJob(*this), true);
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
        threadPool->threadPool.removeAllJobs(true, delayTimeMs);
        startTimer(delayTimeMs);
    }

    void timerCallback() override
    {
        stopTimer();
        handleResampling();
    }

    friend class ResamplingJob;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CachedImageResampler)
    JUCE_DECLARE_WEAK_REFERENCEABLE(CachedImageResampler)
};