#ifndef SCENE_HH
#define SCENE_HH

#include "AbstractScene.hh"
#include <unordered_map>
#include <string>

/*!
 * \class Scene
 * \brief Implements AbstractScene and manages mobile objects.
 */
class Scene : public AbstractScene {
private:
    std::unordered_map<std::string, AbstractMobileObj*> objects; //!< Stores mobile objects

public:
    ~Scene() override {
        for (auto& obj : objects) {
            delete obj.second; // Clean up memory
        }
    }

    // Add a new mobile object
    virtual void AddMobileObj(AbstractMobileObj* pMobObj) override {
        if (pMobObj) {
            objects[pMobObj->GetName()] = pMobObj;
        }
    }

    // Find a mobile object by name
    virtual AbstractMobileObj* FindMobileObj(const char* sName) override {
        auto it = objects.find(sName);
        return (it != objects.end()) ? it->second : nullptr;
    }
};

#endif
