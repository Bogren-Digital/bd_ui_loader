namespace BogrenDigital::UILoading {

KnobComponent::KnobComponent(const juce::String& name,juce::OwnedArray<juce::Image>& imagesToUse, UILoader::ComponentMetadata metadata, juce::Image maskImage)
: juce::Slider(name)
, DeferredImageResampler(*dynamic_cast<juce::Component*>(this), std::move(maskImage))
{
    images.swapWith(imagesToUse); // Transfer ownership of images
    setSliderStyle(juce::Slider::RotaryVerticalDrag);
    setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    setOpaque(false);
}

void KnobComponent::paint(juce::Graphics& g)
{
    if (images.size() > 0)
    {
        const auto normalizedValue = (getValue() - getMinimum()) / (getMaximum() - getMinimum());
        const auto imageIndex = static_cast<int>(normalizedValue * (images.size() - 1));
        drawImage(g, imageIndex);
    }
    else
    {
        g.fillAll(juce::Colours::darkgrey.withAlpha(0.3f));
        g.setColour(juce::Colours::white);
        g.drawText("No Images Provided", getLocalBounds(),
                   juce::Justification::centred, true);
    }
}

} // namespace BogrenDigital::UILoading
