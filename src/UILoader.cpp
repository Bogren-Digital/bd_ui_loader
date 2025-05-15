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

class PlaceholderComponent  : public juce::Component
                            , public PlayfulTones::ComponentResizer
                            , public OriginalSizeReporter
{
public:
    PlaceholderComponent(const juce::String& name, UILoader::ComponentMetadata metadata)
    : juce::Component(name)
    , ComponentResizer(*dynamic_cast<juce::Component*>(this))
    , OriginalSizeReporter(std::move(metadata))
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
        sourceBounds = juce::Rectangle<float>(0.0f, 0.0f, static_cast<float>(bitmapWidth), static_cast<float>(bitmapHeight));
        
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

static juce::String getResourceName(const juce::String& filename)
{
    // Convert filename to BinaryData resource name (e.g., "Background.png" to "Background_png")
    return filename.replaceCharacter('.', '_');
}

static juce::Image loadImageFromBinaryData(const juce::String& resourceName)
{
    // Get the image data from BinaryData
    int dataSize = 0;
    const char* imageData = BinaryData::getNamedResource(resourceName.toRawUTF8(), dataSize);
    
    // Create image from the binary data
    return (imageData != nullptr && dataSize > 0) ? 
           juce::ImageFileFormat::loadFrom(imageData, (size_t)dataSize) : 
           juce::Image();
}

void UILoader::createComponent(const ComponentMetadata& metadata)
{
    // For now, just a placeholder for component creation
    // This will be replaced with actual component instantiation later
    juce::Component* component = nullptr;
    
    if (metadata.type == "IMAGE")
    {
        juce::Image image = loadImageFromBinaryData(getResourceName(metadata.file));
            
        // Create the image component with the loaded image
        component = new ImageComponent(metadata.name, image, metadata);
    }
    else if (metadata.type == "TWEENABLE")
    {
        juce::Image image = loadImageFromBinaryData(getResourceName(metadata.file));
            
        // Create the tweenable component with the loaded image
        auto tweenableComp = new TweenableComponent(metadata.name, image, metadata);
        component = tweenableComp;
        
        // Set up the callback for position updates when the normalized value changes
        tweenableComp->onNewPositionNeeded = [this, tweenableComp](float normalizedValue)
        {
            juce::ignoreUnused(normalizedValue);
            applyLayoutToComponent(tweenableComp);
        };
        
        // Initialize with a default value
        tweenableComp->setNormalizedValue(0.5f);
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
                juce::Image image = loadImageFromBinaryData(getResourceName(imageName));
                knobImages.add(new juce::Image(image));
            }
            
            // Create the knob component with the loaded images
            if (knobImages.size() > 0)
            {
                component = new KnobComponent(metadata.name, knobImages, metadata);
                
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
                component = new PlaceholderComponent(metadata.name, metadata);
            }
        }
        else if (metadata.componentType == "Buttons")
        {
            // Create an array to hold all the button frame images
            auto buttonImages = juce::OwnedArray<juce::Image>();
            
            // Load all frame images
            for (int i = 0; i < metadata.numberOfFrames; ++i)
            {
                // Construct the image name: prefix + index + suffix
                juce::String imageName = metadata.fileNamePrefix + juce::String(i) + metadata.fileNameSuffix;
                juce::Image image = loadImageFromBinaryData(getResourceName(imageName));
                buttonImages.add(new juce::Image(image));
            }
            
            // Create the radio button group component with the loaded images
            if (buttonImages.size() > 0)
            {
                component = new RadioButtonGroup(metadata.name, buttonImages, metadata);
            }
            else
            {
                // Fallback if no images could be loaded
                component = new PlaceholderComponent(metadata.name, metadata);
            }
        }
        else
        {
            component = new PlaceholderComponent(metadata.name, metadata);
        }
    }
    
    if (component != nullptr)
    {
        component->setBounds(metadata.x, metadata.y, metadata.width, metadata.height);
        parentComponent.addAndMakeVisible(component);
        components.add(component);
    }
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
