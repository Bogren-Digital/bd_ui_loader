namespace BogrenDigital::UILoading
{
PlaceholderComponent::PlaceholderComponent(const juce::String& name, UILoader::ComponentMetadata metadata)
: juce::Component(name)
{
    setOpaque(false);
}

void PlaceholderComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::lightgrey.withAlpha(0.2f));
    g.setColour(juce::Colours::black);
    g.drawText(getName(), getLocalBounds(), juce::Justification::centred, true);
}
}
