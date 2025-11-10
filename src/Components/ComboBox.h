#pragma once

namespace BogrenDigital::UILoading {

/**
 * @brief A dropdown menu component for selecting from a list of options.
 *
 * Extends juce::ComboBox with metadata-based initialization.
 */
class ComboBox : public juce::ComboBox
{
public:
    explicit ComboBox(UILoader::ComponentMetadata metadata);
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComboBox)
};

} // namespace BogrenDigital::UILoading
