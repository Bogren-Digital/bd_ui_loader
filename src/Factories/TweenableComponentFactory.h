class TweenableComponentFactory : public ComponentFactory
{
public:
    explicit TweenableComponentFactory(UILoader::ComponentMetadata m, UILoader& loader)
        : ComponentFactory(std::move(m))
        , uiLoader(loader)
    {
    }

    juce::Component* createComponent() override
    {
        juce::Image image = BinaryAssetLoading::loadImageFromBinaryData(BinaryAssetLoading::getResourceName(metadata.file));
            
        // Create the tweenable component with the loaded image
        TweenableComponent* component = new TweenableComponent(metadata.name, image, metadata);
        
        // Set up the callback for position updates when the normalized value changes
        component->onNewPositionNeeded = [this, component](float normalizedValue)
        {
            juce::ignoreUnused(normalizedValue);
            uiLoader.applyLayoutToComponent(component);
        };

        return component;
    }

private:
    UILoader& uiLoader;
};