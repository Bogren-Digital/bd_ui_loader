UILoader::UILoader(juce::Component& parent)
    : parentComponent(parent)
{
}

UILoader::~UILoader()
{
}

void UILoader::loadUI()
{
    // Load XML from binary data
    juce::String xmlContent = juce::String::createStringFromData(
        BinaryData::metadata_xml, BinaryData::metadata_xmlSize);
    
    parseXML(xmlContent);
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

void UILoader::createComponent(const ComponentMetadata& metadata)
{
    // For now, just a placeholder for component creation
    // This will be replaced with actual component instantiation later
    juce::Component* component = nullptr;
    
    if (metadata.type == "IMAGE")
    {
        // Create image component (placeholder)
        component = new juce::Component();
        component->setName(metadata.name);
    }
    else if (metadata.type == "TWEENABLE")
    {
        // Create tweenable component (placeholder)
        component = new juce::Component();
        component->setName(metadata.name);
    }
    else if (metadata.type == "GROUP")
    {
        // Create group component based on type (placeholder)
        if (metadata.componentType == "Knobs")
        {
            component = new juce::Component();
        }
        else if (metadata.componentType == "Buttons")
        {
            component = new juce::Component();
        }
        else
        {
            component = new juce::Component();
        }
        component->setName(metadata.name);
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
    // For each component, apply the stored position and size
    for (int i = 0; i < metadataList.size() && i < components.size(); ++i)
    {
        auto& metadata = metadataList.getReference(i);
        components[i]->setBounds(metadata.x, metadata.y, metadata.width, metadata.height);
    }
}
