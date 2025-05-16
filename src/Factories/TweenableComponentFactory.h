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
        UILoader* uiBuilder = &uiLoader;
        
        // Set up the callback for position updates when the normalized value changes
        component->onNewPositionNeeded = [uiBuilder, component](float normalizedValue)
        {
            juce::ignoreUnused(normalizedValue);

            if(component != nullptr && uiBuilder != nullptr)
            {
                uiBuilder->applyLayoutToComponent(component);
            }
            else
            {
                jassertfalse; // Component should not be null here
            }
        };

        return component;
    }

private:
    UILoader& uiLoader;
};