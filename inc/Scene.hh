#ifndef SCENE_HH
#define SCENE_HH

#include "AbstractScene.hh"
#include <unordered_map>
#include <string>
#include <mutex>

/*!
 * \class Scene
 * \brief Implements AbstractScene and manages mobile objects.
 */
class Scene : public AbstractScene {
private:
    std::unordered_map<std::string, AbstractMobileObj*> objects; //!< Stores mobile objects
    std::mutex sceneMutex; //!< Mutex for synchronizing access

public:
    ~Scene() override {
        for (auto& obj : objects) {
            delete obj.second; // Clean up memory
        }
    }

    /*!
     * \brief Add a new mobile object to the scene.
     * \param pMobObj Pointer to the mobile object to add.
     */
    void AddMobileObj(AbstractMobileObj* pMobObj) override {
        if (pMobObj) {
            std::lock_guard<std::mutex> lock(sceneMutex); // Lock the scene while modifying
            objects[pMobObj->GetName()] = pMobObj;
        }
    }

    /*!
     * \brief Find a mobile object by its name.
     * \param sName The fully qualified name of the object.
     * \return Pointer to the object if found, nullptr otherwise.
     */
    AbstractMobileObj* FindMobileObj(const char* sName) override {
        std::lock_guard<std::mutex> lock(sceneMutex); // Lock the scene while accessing
        auto it = objects.find(sName);
        return (it != objects.end()) ? it->second : nullptr;
    }

    /*!
     * \brief Provides access to the scene's mutex for external synchronization.
     * \return Reference to the scene's mutex.
     */
    std::mutex& GetMutex() override {
        return sceneMutex;
    }
};

#endif

