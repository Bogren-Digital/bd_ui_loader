class ComponentFactory
{
public:
    explicit ComponentFactory(UILoader::ComponentMetadata componentMetadata)
        : metadata(std::move(componentMetadata))
    {
    }

    virtual ~ComponentFactory() = default;
    virtual juce::Component* createComponent() = 0;

protected:
    UILoader::ComponentMetadata metadata;
};