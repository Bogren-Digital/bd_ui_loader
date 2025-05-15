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

class PlaceholderComponent : public juce::Component, public PlayfulTones::ComponentResizer
{
public:
    PlaceholderComponent(const juce::String& name) : juce::Component(name), ComponentResizer(*dynamic_cast<juce::Component*>(this))
    {
        setOpaque(false);
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::lightgrey.withAlpha(0.2f));
        g.setColour(juce::Colours::black);
        g.drawText(getName(), getLocalBounds(), juce::Justification::centred, true);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaceholderComponent)
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
            createComponent(metadata);
        }
    }
}

static juce::String getResourceName(const juce::String& filename)
{
    // Convert filename to BinaryData resource name (e.g., "Background.png" to "Background_png")
    return filename.replaceCharacter('.', '_');
}

void UILoader::createComponent(const ComponentMetadata& metadata)
{
    // For now, just a placeholder for component creation
    // This will be replaced with actual component instantiation later
    juce::Component* component = nullptr;
    
    if (metadata.type == "IMAGE")
    {
        juce::String resourceName = getResourceName(metadata.file);
        
        // Get the image data from BinaryData
        int dataSize = 0;
        const char* imageData = BinaryData::getNamedResource(resourceName.toRawUTF8(), dataSize);

        // Create image from the binary data
        juce::Image image = (imageData != nullptr && dataSize > 0) ? juce::ImageFileFormat::loadFrom(imageData, (size_t)dataSize) : juce::Image();
            
        // Create the image component with the loaded image
        component = new ImageComponent(metadata.name, image);
    }
    else if (metadata.type == "TWEENABLE")
    {
        // Create tweenable component (placeholder)
        component = new PlaceholderComponent(metadata.name);
    }
    else if (metadata.type == "GROUP")
    {
        // Create group component based on type
        if (metadata.componentType == "Knobs")
        {
            // Create an array to hold all the knob frame images
            auto knobImages = juce::OwnedArray<juce::Image>();
            
            // Load all frame images
            for (int i = 0; i < metadata.numberOfFrames; ++i)
            {
                // Construct the image name: prefix + index + suffix
                juce::String imageName = metadata.fileNamePrefix + juce::String(i) + metadata.fileNameSuffix;
                juce::String resourceName = getResourceName(imageName);
                
                // Get the image data from BinaryData
                int dataSize = 0;
                const char* imageData = BinaryData::getNamedResource(resourceName.toRawUTF8(), dataSize);
                
                // Create image from the binary data and add it to the array
                if (imageData != nullptr && dataSize > 0)
                {
                    auto image = new juce::Image(juce::ImageFileFormat::loadFrom(imageData, (size_t)dataSize));
                    knobImages.add(image);
                }
            }
            
            // Create the knob component with the loaded images
            if (knobImages.size() > 0)
            {
                component = new KnobComponent(metadata.name, knobImages);
                
                // Configure the slider ranges
                if (auto* knob = dynamic_cast<KnobComponent*>(component))
                {
                    knob->setRange(0.0, 1.0);
                    knob->setValue(0.5, juce::dontSendNotification);
                }
            }
            else
            {
                // Fallback if no images could be loaded
                component = new PlaceholderComponent(metadata.name);
            }
        }
        else if (metadata.componentType == "Buttons")
        {
            component = new PlaceholderComponent(metadata.name);
        }
        else
        {
            component = new PlaceholderComponent(metadata.name);
        }
    }
    
    if (component != nullptr)
    {
        component->setBounds(metadata.x, metadata.y, metadata.width, metadata.height);
        parentComponent.addAndMakeVisible(component);
        components.add(component);
    }
}

void UILoader::applyLayout()
{
    // Skip if bitmap dimensions are invalid
    if (bitmapWidth <= 0 || bitmapHeight <= 0)
        return;
        
    // Create source rectangle (original bitmap size)
    juce::Rectangle<float> sourceBounds(0.0f, 0.0f, static_cast<float>(bitmapWidth), static_cast<float>(bitmapHeight));
    
    // Create target rectangle (current parent component size)
    juce::Rectangle<float> targetBounds(0.0f, 0.0f, 
                                       static_cast<float>(parentComponent.getWidth()), 
                                       static_cast<float>(parentComponent.getHeight()));
    
    // For each component, apply transformed bounds using ComponentResizer
    for (int i = 0; i < metadataList.size() && i < components.size(); ++i)
    {
        auto& metadata = metadataList.getReference(i);
        
        // Create the component's original bounds as a float rectangle
        juce::Rectangle<float> componentSourceBounds(
            static_cast<float>(metadata.x), 
            static_cast<float>(metadata.y),
            static_cast<float>(metadata.width), 
            static_cast<float>(metadata.height));
            
        // Get the transform that would map the source bounds to target bounds
        juce::AffineTransform transform = PlayfulTones::ComponentResizer::getRectTransform(sourceBounds, targetBounds);
        
        // Apply the transform to the component's source bounds
        juce::Rectangle<float> transformedBounds = componentSourceBounds.transformedBy(transform);
        
        // Check if the component has smooth resizing enabled
        if (auto* resizer = dynamic_cast<PlayfulTones::ComponentResizer*>(components[i]))
        {
            // Use ComponentResizer for smooth resizing
            resizer->setTransformedBounds(transformedBounds);
        }
    }
}
