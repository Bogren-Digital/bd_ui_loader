class PlaceholderComponentFactory : public ComponentFactory
{
public:
    explicit PlaceholderComponentFactory(UILoader::ComponentMetadata m)
        : ComponentFactory(std::move(m))
    {
    }

    juce::Component* createComponent() override
    {
        return new PlaceholderComponent(metadata.name, metadata);
    }
};