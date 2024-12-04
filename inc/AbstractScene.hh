#ifndef ABSTRACTSCENE_HH
#define ABSTRACTSCENE_HH

#include "AbstractMobileObj.hh"
#include <mutex>

/*!
 * \class AbstractScene
 * \brief Abstract interface for managing a scene with mobile objects.
 */
class AbstractScene {
public:
    virtual ~AbstractScene() {}

    /*!
     * \brief Find a mobile object by its name.
     * \param sName The fully qualified name of the object.
     * \return Pointer to the object if found, nullptr otherwise.
     */
    virtual AbstractMobileObj* FindMobileObj(const char* sName) = 0;

    /*!
     * \brief Add a new mobile object to the scene.
     * \param pMobObj Pointer to the mobile object to add.
     */
    virtual void AddMobileObj(AbstractMobileObj* pMobObj) = 0;

    /*!
     * \brief Provides access to the mutex for synchronization.
     * \return Reference to the scene's mutex.
     */
    virtual std::mutex& GetMutex() = 0;
};

#endif
