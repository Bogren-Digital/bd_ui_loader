#pragma once

namespace BogrenDigital::UILoading
{

    /**
     * @brief A static image component with high-quality resampling.
     *
     * Displays a single image using DeferredImageResampler for
     * sub-pixel accuracy and smooth scaling.
     */
    class ImageComponent : public juce::Component,
                           public BogrenDigital::ImageResampler::DeferredImageResampler
    {
    public:
        ImageComponent (const juce::String& name, const juce::Image& imageToUse, UILoader::ComponentMetadata metadata, juce::Image maskImage = {});
        void paint (juce::Graphics& g) override;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImageComponent)
    };

} // namespace BogrenDigital::UILoading
