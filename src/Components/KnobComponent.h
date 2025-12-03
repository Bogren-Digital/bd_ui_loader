#pragma once

namespace BogrenDigital::UILoading
{

    /**
     * @brief A rotary knob control that displays filmstrip images.
     *
     * Combines Slider functionality with image-based rendering using
     * DeferredImageResampler for high-quality scaling.
     */
    class KnobComponent : public juce::Slider,
                          public BogrenDigital::ImageResampler::DeferredImageResampler
    {
    public:
        KnobComponent (const juce::String& name, juce::OwnedArray<juce::Image>& imagesToUse, UILoader::ComponentMetadata metadata, juce::Image maskImage, juce::Image hitboxMaskImage = {});
        void paint (juce::Graphics& g) override;
        bool hitTest (int x, int y) override;

    private:
        juce::Image hitboxMask;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KnobComponent)
    };

} // namespace BogrenDigital::UILoading
