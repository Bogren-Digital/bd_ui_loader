class AspectRatioListener : public juce::ComponentListener
{
public:
    AspectRatioListener(double ratio) : aspectRatio(ratio) {}
    
    void componentMovedOrResized (juce::Component& component, bool wasMoved, bool wasResized) override
    {
        juce::ignoreUnused(wasMoved);
        if (! wasResized)
        {
            return; // Only handle resize events
        }
        
        // Get current size
        int width = component.getWidth();
        int height = component.getHeight();
        
        // Calculate the target height based on current width and aspect ratio
        int targetHeight = static_cast<int>(width / aspectRatio);
        
        // If height doesn't match the aspect ratio, adjust it
        if (height != targetHeight)
        {
            component.setSize(width, targetHeight);
        }
    }
    
private:
    double aspectRatio;
};

void UILoader::applyProportionalResize()
{
    // Only apply if we have valid bitmap dimensions
    if (bitmapWidth > 0 && bitmapHeight > 0)
    {
        // Initially set parent to the bitmap size
        parentComponent.setSize(bitmapWidth, bitmapHeight);
        
        // Store the aspect ratio for future resizing
        double aspectRatio = static_cast<double>(bitmapWidth) / static_cast<double>(bitmapHeight);
        
        // Create and attach the listener to maintain aspect ratio during resizes
        aspectRatioListener = std::make_unique<AspectRatioListener>(aspectRatio);
        parentComponent.addComponentListener(aspectRatioListener.get());
    }
}

UILoader::UILoader(juce::Component& parent)
    : parentComponent(parent)
{
}

UILoader::~UILoader()
{
    if (aspectRatioListener != nullptr)
    {
        parentComponent.removeComponentListener(aspectRatioListener.get());
    }
}

void UILoader::loadUI()
{
    // Load XML from binary data
    juce::String xmlContent = juce::String::createStringFromData(
        BinaryData::metadata_xml, BinaryData::metadata_xmlSize);
    
    parseXML(xmlContent);
    
    // Apply proportional resize constraint based on bitmap dimensions
    applyProportionalResize();
    
    applyLayout();
}

void UILoader::parseXML(const juce::String& xmlContent)
{
    auto xmlDocument = juce::parseXML(xmlContent);
    
    if (xmlDocument != nullptr)
    {
        // Clear any previous metadata
        metadataList.clear();
        components.clear();
        
        // Get the root element dimensions
        bitmapWidth = xmlDocument->getIntAttribute("width", 0);
        bitmapHeight = xmlDocument->getIntAttribute("height", 0);
        sourceBounds = juce::Rectangle<float>(0.0f, 0.0f, static_cast<float>(bitmapWidth), static_cast<float>(bitmapHeight));

        auto componentFactories = std::vector<std::unique_ptr<ComponentFactory>>();
        
        // Process all child elements
        for (auto* element : xmlDocument->getChildIterator())
        {
            ComponentMetadata metadata;
            
            // Get the tag type
            metadata.type = element->getTagName();
            
            // Common attributes
            metadata.name = element->getStringAttribute("name", "");
            metadata.x = element->getIntAttribute("x", 0);
            metadata.y = element->getIntAttribute("y", 0);
            metadata.width = element->getIntAttribute("width", 0);
            metadata.height = element->getIntAttribute("height", 0);
            
            // Specific attributes based on type
            if (metadata.type == "IMAGE")
            {
                metadata.file = element->getStringAttribute("file", "");
            }
            else if (metadata.type == "TWEENABLE")
            {
                metadata.file = element->getStringAttribute("file", "");
                metadata.minX = element->getIntAttribute("minX", 0);
                metadata.minY = element->getIntAttribute("minY", 0);
                metadata.maxX = element->getIntAttribute("maxX", 0);
                metadata.maxY = element->getIntAttribute("maxY", 0);
                metadata.x = metadata.minX;
                metadata.y = metadata.minY;
            }
            else if (metadata.type == "GROUP")
            {
                metadata.numberOfFrames = element->getIntAttribute("numberOfFrames", 0);
                metadata.fileNamePrefix = element->getStringAttribute("fileNamePrefix", "");
                metadata.fileNameSuffix = element->getStringAttribute("fileNameSuffix", "");
                metadata.componentType = element->getStringAttribute("type", "");
            }
            
            metadataList.add(metadata);
            componentFactories.push_back(createComponentFactory(metadata));
        }

        // Create components based on metadata
        for (auto& factory : componentFactories)
        {
            auto* component = factory->createComponent();
            if (component != nullptr)
            {
                parentComponent.addAndMakeVisible(component);
                components.add(component);
                
                // Apply layout to the component
                applyLayoutToComponent(component);
            }
        }
    }
}

// Helper method to calculate transformed bounds for a component
juce::Rectangle<float> UILoader::calculateTransformedBounds(
    const juce::Rectangle<float>& sourceBounds,
    const juce::Rectangle<float>& targetBounds,
    const juce::Rectangle<float>& componentSourceBounds)
{
    // Get the transform that would map the source bounds to target bounds
    juce::AffineTransform transform = PlayfulTones::ComponentResizer::getRectTransform(sourceBounds, targetBounds);
    
    // Apply the transform to the component's source bounds
    return componentSourceBounds.transformedBy(transform);
}

std::unique_ptr<ComponentFactory> UILoader::createComponentFactory(const ComponentMetadata& metadata)
{
    if (metadata.type == "IMAGE")
    {
        return std::make_unique<ImageComponentFactory>(metadata);
    }
    else if (metadata.type == "TWEENABLE")
    {
        return std::make_unique<TweenableComponentFactory>(metadata, *this);
    }
    else if (metadata.type == "GROUP")
    {
        // Create group component based on type
        if (metadata.componentType == "Knobs")
        {
            return std::make_unique<KnobComponentFactory>(metadata);
        }
        else if (metadata.componentType == "Buttons")
        {
            return std::make_unique<RadioButtonGroupFactory>(metadata);
        }
    }
    
    return std::make_unique<PlaceholderComponentFactory>(metadata);
}

void UILoader::applyLayoutToComponent(juce::Component* component)
{
    OriginalSizeReporter* originalSizeReporter = dynamic_cast<OriginalSizeReporter*>(component);
    if (originalSizeReporter != nullptr)
    {
        // Get the component's original bounds as a float rectangle
        juce::Rectangle<float> componentSourceBounds = originalSizeReporter->getComponentSourceBounds();
        
        juce::Rectangle<float> transformedBounds = calculateTransformedBounds(
            sourceBounds, targetBounds, componentSourceBounds);
        
        // Check if the component has smooth resizing enabled
        if (auto* resizer = dynamic_cast<PlayfulTones::ComponentResizer*>(component))
        {
            resizer->setTransformedBounds(transformedBounds);
        }
    }    
}

void UILoader::applyLayout()
{
    // Skip if bitmap dimensions are invalid
    if (bitmapWidth <= 0 || bitmapHeight <= 0)
        return;
    
    // Update target rectangle (current parent component size)
    targetBounds = juce::Rectangle<float> (0.0f, 0.0f, 
                static_cast<float>(parentComponent.getWidth()), 
                static_cast<float>(parentComponent.getHeight()));
    
    for (int i = 0; i < components.size(); ++i)
    {
        applyLayoutToComponent(components[i]);
    }
}
