namespace BogrenDigital::UILoading
{
    TweenableComponent::TweenableComponent(const juce::String& name, const juce::Image& imageToUse, UILoader::ComponentMetadata metadata)
    : ImageComponent(name, imageToUse, std::move(metadata))
    , timerCallback(*this)
    {
        timerCallback.callback = [](TweenableComponent& comp) {
            auto& props = comp.getProperties();
            props.set("x", juce::jmap(comp.normalizedValue.load(),
                                static_cast<float>(props["minX"].operator int()),
                                static_cast<float>(props["maxX"].operator int())));
            props.set(" y", juce::jmap(comp.normalizedValue.load(),
                                static_cast<float>(props["minY"].operator int()),
                                static_cast<float>(props["maxY"].operator int())));
        };
    }

    void TweenableComponent::setNormalizedValue(float newValue)
    {
        normalizedValue.store(juce::jlimit(0.0f, 1.0f, newValue));
        onNewPositionNeeded(normalizedValue);
        repaint();
    }

    float TweenableComponent::getNormalizedValue() const
    {
        return normalizedValue.load();
    }
}
