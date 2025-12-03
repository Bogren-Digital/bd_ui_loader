#pragma once

namespace BogrenDigital::UILoading
{
    /**
     * @brief A group of mutually exclusive buttons with filmstrip visualization.
     *
     * Displays different images for each button state while managing
     * the selection logic. Uses invisible toggle buttons for hit detection.
     */
    class RadioButtonGroup : public juce::Component,
                             public BogrenDigital::ImageResampler::DeferredImageResampler
    {
    private:
        /** @brief Custom LookAndFeel that makes toggle buttons invisible. */
        class InvisibleToggleLookAndFeel : public juce::LookAndFeel_V4
        {
        public:
            InvisibleToggleLookAndFeel() = default;
            void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
        };

    public:
        RadioButtonGroup (const juce::String& name, juce::OwnedArray<juce::Image>& imagesToUse, UILoader::ComponentMetadata metadata, juce::Image maskImage, juce::Image hitboxMaskImage = {});
        ~RadioButtonGroup() override;

        void resized() override;
        void paint (juce::Graphics& g) override;

        juce::ToggleButton* getButton (int index);
        int getSelectedButtonIndex() const;
        void setSelectedButtonIndex (int index);

        bool hitTest (int x, int y) override;

    private:
        juce::Image hitboxMask;
        InvisibleToggleLookAndFeel invisibleLookAndFeel;
        juce::OwnedArray<juce::ToggleButton> buttons;
        int selectedButtonIndex = -1;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RadioButtonGroup)
    };
}
