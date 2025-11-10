
namespace BogrenDigital::UILoading {

juce::String ComponentFactoryRegistry::getKey(const juce::String& metadataType, const juce::String& componentType)
{
    if (componentType.isEmpty())
        return metadataType;
    return metadataType + ":" + componentType;
}

} // namespace BogrenDigital::UILoading
