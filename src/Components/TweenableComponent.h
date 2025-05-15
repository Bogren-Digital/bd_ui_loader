#pragma once

class TweenableComponent : public ImageComponent
{
public:
    TweenableComponent(const juce::String& name, const juce::Image& imageToUse, UILoader::ComponentMetadata metadata)
    : ImageComponent(name, imageToUse, std::move(metadata))
    {
    }

    void setNormalizedValue(float newValue)
    {
        normalizedValue.store(juce::jlimit(0.0f, 1.0f, newValue));
        onNewPositionNeeded(normalizedValue);
        repaint();
    }

    float getNormalizedValue() const
    {
        return normalizedValue.load();
    }

    std::function<void(float)> onNewPositionNeeded = [](float newValue)
    {
        juce::ignoreUnused(newValue);
        // Default implementation does nothing
    };

    const juce::Rectangle<float>& getComponentSourceBounds() override
    {
        // Calculate the interpolated position based on normalized value
        float x = juce::jmap(normalizedValue.load(),
                            static_cast<float>(componentMetadata.minX),
                            static_cast<float>(componentMetadata.maxX));
        float y = juce::jmap(normalizedValue.load(), 
                            static_cast<float>(componentMetadata.minY),
                            static_cast<float>(componentMetadata.maxY));
        
        setComponentSourceBounds(juce::Rectangle<float>(
            x,
            y,
            static_cast<float>(componentMetadata.width),
            static_cast<float>(componentMetadata.height)));
        return componentSourceBounds;
    }

private:
    std::atomic<float> normalizedValue{ 0.0f };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TweenableComponent)
};
