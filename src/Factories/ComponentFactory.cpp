namespace BogrenDigital::UILoading
{

juce::Component* ComponentFactory::createComponent (const juce::XmlElement* element)
{
    auto metadata = UILoader::parseElement (element);
    return createComponent (metadata);
}

juce::Component* ComponentFactory::createComponent (const UILoader::ComponentMetadata& metadata)
{
    return new PlaceholderComponent (metadata.name, metadata);
}

} // namespace BogrenDigital::UILoading
