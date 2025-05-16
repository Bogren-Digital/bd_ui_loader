class PlaceholderComponent  : public juce::Component
                            , public PlayfulTones::ComponentResizer
                            , public OriginalSizeReporter
{
public:
    PlaceholderComponent(const juce::String& name, UILoader::ComponentMetadata metadata)
    : juce::Component(name)
    , ComponentResizer(*dynamic_cast<juce::Component*>(this))
    , OriginalSizeReporter(std::move(metadata))
    {
        setOpaque(false);
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::lightgrey.withAlpha(0.2f));
        g.setColour(juce::Colours::black);
        g.drawText(getName(), getLocalBounds(), juce::Justification::centred, true);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaceholderComponent)
};