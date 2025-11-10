namespace BogrenDigital::UILoading {

void HooverableSwitchComponent::SwitchLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                         bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    juce::ignoreUnused(shouldDrawButtonAsDown);
    if(!shouldDrawButtonAsHighlighted)
        return;

    // Check the toggle state and draw the appropriate image
    if (images != nullptr)
    {
        int imageIndex = 0;
        if (imageIndex < images->size() && (*images)[imageIndex] != nullptr && (*images)[imageIndex]->isValid())
        {
            g.drawImage(*(*images)[imageIndex], button.getLocalBounds().toFloat());
            return;
        }
    }
    // Fallback if no valid image or images array is empty
    g.fillAll(juce::Colours::darkgrey.withAlpha(0.3f));
    g.setColour(juce::Colours::white);
    g.drawText("Switch: " + button.getName(), button.getLocalBounds(),
                juce::Justification::centred, true);
}

HooverableSwitchComponent::HooverableSwitchComponent(const juce::String& name, juce::OwnedArray<juce::Image>& imagesToUse, UILoader::ComponentMetadata metadata)
: juce::ToggleButton(name)
{
    images.swapWith(imagesToUse); // Transfer ownership of images

    // Set our custom look and feel
    switchLookAndFeel.setImages(&images);
    setLookAndFeel(&switchLookAndFeel);

    // Default state is off (first image)
    setToggleState(false, juce::dontSendNotification);

    setOpaque(false);
}

HooverableSwitchComponent::~HooverableSwitchComponent()
{
    // Reset the LookAndFeel to avoid dangling pointers
    setLookAndFeel(nullptr);
}

} // namespace BogrenDigital::UILoading
