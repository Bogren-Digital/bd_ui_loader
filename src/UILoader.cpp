namespace BogrenDigital::UILoading
{

    class AspectRatioListener : public juce::ComponentListener
    {
    public:
        AspectRatioListener (double ratio) : aspectRatio (ratio) {}

        void componentMovedOrResized (juce::Component& component, bool wasMoved, bool wasResized) override
        {
            juce::ignoreUnused (wasMoved);
            if (!wasResized)
                return;

            int width = component.getWidth();
            int height = component.getHeight();
            int targetHeight = static_cast<int> (width / aspectRatio);

            if (height != targetHeight)
                component.setSize (width, targetHeight);
        }

    private:
        double aspectRatio;
    };

    void UILoader::applyProportionalResize()
    {
        if (bitmapLayout.width > 0 && bitmapLayout.height > 0)
        {
            parentComponent.setSize (bitmapLayout.width, bitmapLayout.height);

            // Store the aspect ratio for future resizing
            double aspectRatio = bitmapLayout.getAspectRatio();

            // Create and attach the listener to maintain aspect ratio during resizes
            aspectRatioListener = std::make_unique<AspectRatioListener> (aspectRatio);
            parentComponent.addComponentListener (aspectRatioListener.get());
        }
    }

    UILoader::UILoader (juce::Component& parent, BinaryAssetImageLoader& imgLoader)
        : parentComponent (parent), imageLoader (imgLoader), componentFactoryRegistry (std::make_unique<ComponentFactoryRegistry>())
    {
        registerComponentFactories();
    }

    UILoader::~UILoader()
    {
        if (aspectRatioListener != nullptr)
        {
            parentComponent.removeComponentListener (aspectRatioListener.get());
        }
    }

    void UILoader::loadUI (const juce::String& xmlFileName)
    {
        const auto xmlContent = imageLoader.getStringFromAsset (xmlFileName);
        parseXML (xmlContent);
        applyProportionalResize();
        applyLayout();
    }

    UILoader::ComponentMetadata UILoader::parseElement (juce::XmlElement* element)
    {
        ComponentMetadata metadata;

// Parse all fields from XML attributes using the macro
#define STRING_FIELD(fieldName, defaultValue) metadata.fieldName = element->getStringAttribute (#fieldName, defaultValue);
#define INT_FIELD(fieldName, defaultValue) metadata.fieldName = element->getIntAttribute (#fieldName, defaultValue);
#define BOOL_FIELD(fieldName, defaultValue) metadata.fieldName = element->getBoolAttribute (#fieldName, defaultValue);
        COMPONENT_METADATA_FIELDS
#undef STRING_FIELD
#undef INT_FIELD
#undef BOOL_FIELD

        // Special case: type comes from tag name, not attribute
        metadata.type = element->getTagName();

        if (metadata.type == "TWEENABLE")
        {
            metadata.x = metadata.minX;
            metadata.y = metadata.minY;
        }

        return metadata;
    }

// Clean up the macro now that we're done with it
#undef COMPONENT_METADATA_FIELDS

    void UILoader::parseXML (const juce::String& xmlContent)
    {
        auto xmlDocument = juce::parseXML (xmlContent);

        if (xmlDocument != nullptr)
        {
            components.clear();
            componentsByName.clear();

            bitmapLayout.setDimensions (
                xmlDocument->getIntAttribute ("width", 0),
                xmlDocument->getIntAttribute ("height", 0));

            for (auto* element : xmlDocument->getChildIterator())
            {
                const auto metadata = parseElement (element);

                if (componentsByName.contains (metadata.name))
                {
                    jassertfalse; // Duplicate component name detected
                    juce::Logger::writeToLog ("Duplicate component name detected: " + metadata.name + " - skipping component");
                    continue;
                }

                if (auto* factory = componentFactoryRegistry->getFactory (metadata, this); factory != nullptr)
                {
                    if (auto* component = factory->createComponent (metadata); component != nullptr)
                    {
                        parentComponent.addAndMakeVisible (component);
                        components.add (component);
                        componentsByName[metadata.name] = component;
                        applyMetadataToProperties (component, metadata);
                        applyLayoutToComponent (component);
                    }
                }
                else
                {
                    juce::Logger::writeToLog ("No factory found for component type: " + metadata.type);
                }
            }
        }
    }

    juce::Rectangle<float> UILoader::calculateTransformedBounds (
        const juce::Rectangle<float>& sourceBounds,
        const juce::Rectangle<float>& targetBounds,
        const juce::Rectangle<float>& componentSourceBounds)
    {
        juce::AffineTransform transform = PlayfulTones::ComponentResizer::getRectTransform (sourceBounds, targetBounds);
        return componentSourceBounds.transformedBy (transform);
    }

    void UILoader::registerComponentFactories()
    {
        auto& registry = *componentFactoryRegistry;

        // Register basic component factories
        registry.registerFactory<ImageComponentFactory> ( "IMAGE", "raster");
        registry.registerFactory<TweenableComponentFactory> ( "TWEENABLE", "", std::ref (*this));
        registry.registerFactory<ComboBoxFactory> ( "DROPDOWN");

        // Register group-based factories
        registry.registerFactory<KnobComponentFactory> ( "KNOB", "raster");
        registry.registerFactory<KnobComponentFactory> ( "DIAL", "raster");
        registry.registerFactory<RadioButtonGroupFactory> ( "BUTTONS", "raster");
        registry.registerFactory<SwitchFactory> ( "SWITCH", "raster");
        registry.registerFactory<HooverableFactory> ( "HOOVERABLE", "raster");
        registry.registerFactory<HooverableFactory> ( "LED", "raster");

        // Register fallback factory
        registry.registerFactory<PlaceholderComponentFactory> ( "PLACEHOLDER");
    }

    void UILoader::applyLayoutToComponent (juce::Component* component)
    {
        const auto props = component->getProperties();
        const auto compX = props["x"].operator int();
        const auto compY = props["y"].operator int();
        const auto compWidth = props["width"].operator int();
        const auto compHeight = props["height"].operator int();
        const auto componentSourceBounds = juce::Rectangle { static_cast<float> (compX), static_cast<float> (compY), static_cast<float> (compWidth), static_cast<float> (compHeight) };

        juce::Rectangle<float> transformedBounds = calculateTransformedBounds (
            bitmapLayout.sourceBounds, bitmapLayout.targetBounds, componentSourceBounds);

        const auto floatX = transformedBounds.getX();
        const auto floatY = transformedBounds.getY();
        const auto floatWidth = transformedBounds.getWidth();
        const auto floatHeight = transformedBounds.getHeight();

        auto newIntX = juce::roundToInt (std::floor (floatX));
        auto newIntY = juce::roundToInt (std::floor (floatY));
        auto newIntRight = juce::roundToInt (std::ceil (floatX + floatWidth));
        auto newIntBottom = juce::roundToInt (std::ceil (floatY + floatHeight));

        auto fractionalX = juce::jlimit(-1.f, 1.f, floatX - static_cast<float>(newIntX));
        auto fractionalY = juce::jlimit(-1.f, 1.f, floatY - static_cast<float>(newIntY));

        component->setBounds (newIntX, newIntY, newIntRight - newIntX, newIntBottom - newIntY);
        component->resized();

        // Store fractional rect for sub-pixel rendering
        component->getProperties().set("floatX", fractionalX);
        component->getProperties().set("floatY", fractionalY);
        component->getProperties().set("floatW", floatWidth);
        component->getProperties().set("floatH", floatHeight);
    }

    void UILoader::applyLayout()
    {
        if (bitmapLayout.width <= 0 || bitmapLayout.height <= 0)
            return;

        // Update target rectangle (current parent component size)
        bitmapLayout.targetBounds = juce::Rectangle<float> (0.0f, 0.0f, static_cast<float> (parentComponent.getWidth()), static_cast<float> (parentComponent.getHeight()));

        for (int i = 0; i < components.size(); ++i)
            applyLayoutToComponent (components[i]);
    }

} // namespace BogrenDigital::UILoading