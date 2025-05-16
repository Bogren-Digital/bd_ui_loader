#pragma once

class ComponentFactoryRegistry
{
public:
    using FactoryCreator = std::function<std::unique_ptr<ComponentFactory>(const UILoader::ComponentMetadata&, UILoader*)>;
    
    // Singleton access
    static ComponentFactoryRegistry& getInstance()
    {
        static ComponentFactoryRegistry instance;
        return instance;
    }
    
    // Register a factory type with metadata type and component type
    template<typename FactoryType>
    void registerFactory(const std::string& metadataType, const std::string& componentType = "")
    {
        auto key = createKey(metadataType, componentType);
        
        creators[key] = [](const UILoader::ComponentMetadata& metadata, UILoader* loader) {
            if constexpr (std::is_constructible_v<FactoryType, UILoader::ComponentMetadata, UILoader&>) {
                return std::make_unique<FactoryType>(metadata, *loader);
            } else {
                return std::make_unique<FactoryType>(metadata);
            }
        };
    }
    
    // Create a factory for the given metadata
    std::unique_ptr<ComponentFactory> createFactory(
        const UILoader::ComponentMetadata& metadata, 
        UILoader* loader)
    {
        // Try to find a specific factory first (type + componentType)
        auto specificKey = createKey(metadata.type.toStdString(), metadata.componentType.toStdString());
        auto it = creators.find(specificKey);
        
        // If not found, try with just the type
        if (it == creators.end()) {
            auto generalKey = createKey(metadata.type.toStdString(), "");
            it = creators.find(generalKey);
        }
        
        // If found, create the factory
        if (it != creators.end()) {
            return it->second(metadata, loader);
        }
        
        // Default to placeholder factory
        return std::make_unique<PlaceholderComponentFactory>(metadata);
    }
    
private:
    ComponentFactoryRegistry() = default;
    ~ComponentFactoryRegistry() = default;
    
    // Prevent copying
    ComponentFactoryRegistry(const ComponentFactoryRegistry&) = delete;
    ComponentFactoryRegistry& operator=(const ComponentFactoryRegistry&) = delete;
    
    // Create a unique key from metadata type and component type
    std::string createKey(const std::string& metadataType, const std::string& componentType)
    {
        if (componentType.empty()) {
            return metadataType;
        }
        return metadataType + ":" + componentType;
    }
    
    std::unordered_map<std::string, FactoryCreator> creators;
};
