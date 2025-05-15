#pragma once
class AspectRatioListener;

class UILoader
{
public:
    UILoader(juce::Component& parentComponent);
    ~UILoader();
    
    void loadUI();
    void applyLayout();
    
    // Maintains the aspect ratio of the parent component based on bitmap dimensions
    void applyProportionalResize();
    
private:
    struct ComponentMetadata
    {
        juce::String type;  // IMAGE, GROUP, TWEENABLE
        juce::String name;
        juce::String file;
        juce::String fileNamePrefix;
        juce::String fileNameSuffix;
        juce::String componentType; // For GROUP: Knobs, Buttons, etc.
        int x;
        int y;
        int width;
        int height;
        int numberOfFrames;
        int minX;
        int minY;
        int maxX;
        int maxY;
        
        // Not all fields will be used for all component types
        ComponentMetadata() : x(0), y(0), width(0), height(0), 
                             numberOfFrames(0), minX(0), minY(0), 
                             maxX(0), maxY(0) {}
    };
    
    void parseXML(const juce::String& xmlContent);
    void createComponent(const ComponentMetadata& metadata);
    
    juce::Component& parentComponent;
    juce::Array<ComponentMetadata> metadataList;
    juce::OwnedArray<juce::Component> components;
    
    std::unique_ptr<AspectRatioListener> aspectRatioListener;
    
    // Overall bitmap dimensions from XML root node
    int bitmapWidth = 0;
    int bitmapHeight = 0;
};
