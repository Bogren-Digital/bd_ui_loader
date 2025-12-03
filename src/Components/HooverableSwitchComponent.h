#pragma once

namespace BogrenDigital::UILoading {

/**
 * @brief A toggle button with hover state visualization using images.
 *
 * Extends SwitchComponent with hover/highlighted state rendering.
 * Images are displayed based on toggle and hover states.
 */
class HooverableSwitchComponent : public juce::ToggleButton
{
private:
    /** @brief Custom LookAndFeel that draws toggle buttons using the provided images. */
    class SwitchLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        SwitchLookAndFeel() = default;
        void setImages(juce::OwnedArray<juce::Image>* imgs) { images = imgs; }
        void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                            bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    private:
        juce::OwnedArray<juce::Image>* images = nullptr;
    };

public:
    HooverableSwitchComponent(const juce::String& name, juce::OwnedArray<juce::Image>& imagesToUse, UILoader::ComponentMetadata metadata, juce::Image hitboxMaskImage = {});
    ~HooverableSwitchComponent() override;
    bool hitTest(int x, int y) override;

private:
    juce::OwnedArray<juce::Image> images;
    juce::Image hitboxMask;
    SwitchLookAndFeel switchLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HooverableSwitchComponent)
};

} // namespace BogrenDigital::UILoading
