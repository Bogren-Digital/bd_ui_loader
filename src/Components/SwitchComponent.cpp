namespace BogrenDigital::UILoading
{
    void SwitchLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
    {
        // Ignore highlighted/down states as per requirements
        juce::ignoreUnused(shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

        // Check the toggle state and draw the appropriate image
        if (images != nullptr)
        {
            int imageIndex = button.getToggleState() ? 1 : 0;
            if (imageIndex < images->size() && (*images)[imageIndex] != nullptr && (*images)[imageIndex]->isValid())
            {
                dynamic_cast<BogrenDigital::ImageResampler::DeferredImageResampler*>(&button)->drawImage(g, imageIndex);
                return;
            }
        }
        // Fallback if no valid image or images array is empty
        g.fillAll(juce::Colours::darkgrey.withAlpha(0.3f));
        g.setColour(juce::Colours::white);
        g.drawText("Switch: " + button.getName(), button.getLocalBounds(),
                  juce::Justification::centred, true);
    }
}
