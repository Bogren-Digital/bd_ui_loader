#pragma once

namespace BogrenDigital::UILoading
{
/**
 * @brief A fallback component used when no suitable factory is found.
 *
 * Displays component metadata for debugging purposes.
 */
class PlaceholderComponent : public juce::Component
{
public:
    PlaceholderComponent(const juce::String& name, UILoader::ComponentMetadata metadata);
    void paint(juce::Graphics& g) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaceholderComponent)
};
}