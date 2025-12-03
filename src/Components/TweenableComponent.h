#pragma once

namespace BogrenDigital::UILoading
{
    /**
     * @brief An image component that can be animated between positions.
     *
     * Supports interpolating between min/max X and Y coordinates based
     * on a normalized value. Useful for sliders, meters, and other
     * position-based animations.
     */
    class TweenableComponent : public ImageComponent
    {
    public:
        TweenableComponent(const juce::String& name, const juce::Image& imageToUse, UILoader::ComponentMetadata metadata, juce::Image maskImage = {}, juce::Image hitboxMaskImage = {});

        void setNormalizedValue(float newValue);
        float getNormalizedValue() const;

        std::function<void(float)> onNewPositionNeeded = [](float newValue)
        {
            juce::ignoreUnused(newValue);
        };

    private:
        std::atomic<float> normalizedValue{ 0.0f };

        struct TimerCallback : juce::Timer
        {
            explicit TimerCallback(TweenableComponent& owner)
                : comp(owner)
            {
                startTimerHz (30);
            }

            std::function<void(TweenableComponent&)> callback = [](TweenableComponent& /*component*/){};
        private:
            void timerCallback() override
            {
                callback(comp);
            }
            TweenableComponent& comp;
        };

        TimerCallback timerCallback;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TweenableComponent)
    };
}
