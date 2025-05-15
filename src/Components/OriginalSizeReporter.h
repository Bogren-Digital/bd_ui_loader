class OriginalSizeReporter
{
public:
    OriginalSizeReporter(UILoader::ComponentMetadata metadata)
    : componentMetadata(std::move(metadata))
    {
        // Initialize component source bounds based on metadata
        componentSourceBounds.setBounds(
            static_cast<float>(componentMetadata.x),
            static_cast<float>(componentMetadata.y),
            static_cast<float>(componentMetadata.width),
            static_cast<float>(componentMetadata.height));
    }

    virtual const juce::Rectangle<float>& getComponentSourceBounds()
    {
        return componentSourceBounds;
    }

    void setComponentSourceBounds(const juce::Rectangle<float>& newRect)
    {
        componentSourceBounds = newRect;
    }

protected:
    UILoader::ComponentMetadata componentMetadata;
    juce::Rectangle<float> componentSourceBounds { 0.0f, 0.0f, 0.0f, 0.0f };

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OriginalSizeReporter)
};