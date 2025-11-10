#pragma once

namespace BogrenDigital::UILoading
{
    /** @brief Custom LookAndFeel that draws toggle buttons using the provided images. */
    class SwitchLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        SwitchLookAndFeel() = default;
        void setImages (juce::OwnedArray<juce::Image>* imgs) { images = imgs; }
        void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    protected:
        juce::OwnedArray<juce::Image>* images = nullptr;
    };

    template <typename T>
    concept DerivedFromSwitchLookAndFeel = std::is_base_of_v<SwitchLookAndFeel, T> || std::is_same_v<SwitchLookAndFeel, T>;

    /**
     * @brief A toggle button that displays different images for on/off states.
     *
     * Combines ToggleButton functionality with image-based rendering.
     * Supports custom LookAndFeel types derived from SwitchLookAndFeel.
     */
    template <DerivedFromSwitchLookAndFeel LookAndFeelType = SwitchLookAndFeel>
    class SwitchComponent : public juce::ToggleButton,
                            public BogrenDigital::ImageResampler::DeferredImageResampler
    {

    public:
        SwitchComponent (const juce::String& name, juce::OwnedArray<juce::Image>& imagesToUse, UILoader::ComponentMetadata metadata, juce::Image mask)
        : juce::ToggleButton(name)
        , DeferredImageResampler( *dynamic_cast<juce::Component*>(this), std::move(mask))
        {
            this->images.swapWith(imagesToUse); // Transfer ownership of images to inherited member
            switchLookAndFeel.setImages(&this->images);
            setLookAndFeel(&switchLookAndFeel);

            // Default state is off (first image)
            setToggleState(false, juce::dontSendNotification);

            setOpaque(false);
        }

        ~SwitchComponent() override
        {
            // Reset the LookAndFeel to avoid dangling pointers
            setLookAndFeel(nullptr);
        }

    private:
        LookAndFeelType switchLookAndFeel;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SwitchComponent)
    };
}
