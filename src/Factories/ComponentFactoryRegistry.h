#pragma once

namespace BogrenDigital::UILoading {

/**
 * @brief Registry for managing component factory instances.
 *
 * Maps component types to factory instances, supporting lazy instantiation
 * and variadic factory constructors. Use std::ref() to pass reference arguments.
 */
class ComponentFactoryRegistry
{
public:
    using FactoryCreator = std::function<std::unique_ptr<ComponentFactory>(UILoader*)>;

    /**
     * @brief Register a factory type with metadata type and component type.
     *
     * Accepts additional constructor arguments via variadic templates.
     * Use std::ref() to pass references (e.g., std::ref(uiLoader)).
     */
    template<typename FactoryType, typename... Args>
    void registerFactory(const juce::String& metadataType, const juce::String& componentType = "", Args&&... args)
    {
        const auto key = getKey(metadataType, componentType);

        auto argsTuple = std::make_tuple(std::forward<Args>(args)...);

        factories[key] = [argsTuple](UILoader* loader) mutable {
            return std::apply([&loader](auto&&... capturedArgs) {
                return std::make_unique<FactoryType>(
                    loader->getImageLoader(),
                    std::forward<decltype(capturedArgs)>(capturedArgs)...
                );
            }, argsTuple);
        };
    }

    /** @brief Get or create a factory for the given component metadata. */
    ComponentFactory* getFactory(const UILoader::ComponentMetadata& metadata, UILoader* loader)
    {
        auto specificKey = getKey(metadata.type, metadata.imageType);
        auto it = factoryInstances.find(specificKey);

        if (it == factoryInstances.end()) {
            auto generalKey = getKey(metadata.type, "");
            it = factoryInstances.find(generalKey);
        }

        if (it != factoryInstances.end())
            return it->second.get();

        auto creatorIt = factories.find(specificKey);
        if (creatorIt == factories.end())
            creatorIt = factories.find(getKey(metadata.type, ""));

        if (creatorIt != factories.end()) {
            auto factory = creatorIt->second(loader);
            auto* factoryPtr = factory.get();
            factoryInstances[specificKey] = std::move(factory);
            return factoryPtr;
        }

        // Default to placeholder factory
        auto factory = std::make_unique<PlaceholderComponentFactory>(loader->getImageLoader());
        auto* factoryPtr = factory.get();
        factoryInstances[specificKey] = std::move(factory);
        return factoryPtr;
    }

private:
    juce::String getKey(const juce::String& metadataType, const juce::String& componentType);

    std::unordered_map<juce::String, FactoryCreator> factories;
    std::unordered_map<juce::String, std::unique_ptr<ComponentFactory>> factoryInstances;
};

} // namespace BogrenDigital::UILoading
