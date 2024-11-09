#ifndef SET4LIBINTERFACES_HH
#define SET4LIBINTERFACES_HH

#include <vector>
#include <memory>
#include <list>
#include "LibInterface.hh"

/*!
 * \class Set4LibInterfaces
 * \brief Manages a collection of LibInterface objects representing multiple plugins.
 *
 * This class provides methods to load, access, and manage multiple libraries, each
 * represented by a LibInterface object. It allows loading multiple command plugins
 * and accessing specific commands by name.
 */
class Set4LibInterfaces {
public:
    /*!
     * \brief Adds a new library to the collection and loads it.
     * \param libName Name of the library file to load (e.g., "libInterp4Move.so").
     * \param cmdName Name of the command associated with this library.
     * \return True if the library was loaded successfully, false otherwise.
     */
    bool addLibrary(const std::string& libName, const std::string& cmdName) {
        auto libInterface = std::make_unique<LibInterface>(libName, cmdName);
        
        if (!libInterface->loadLibrary()) {  // Load the library and create the command instance
            return false;
        }

        interfaces.push_back(std::move(libInterface));  // Add to the collection

        return true;
    }

    /*!
     * \brief Retrieves a pointer to the LibInterface object by command name.
     * \param cmdName The name of the command to look up.
     * \return Pointer to the LibInterface object if found, nullptr otherwise.
     */
    LibInterface* getInterface(const std::string& cmdName) const {
        for (const auto& interface : interfaces) {
            if (interface->getCommandName() == cmdName) {  // Match the command name
                return interface.get();
            }
        }

        return nullptr;
    }

    /*!
     * \brief Default destructor for Set4LibInterfaces.
     *
     * Uses default destructor behavior to clean up unique_ptrs in the vector.
     */
    ~Set4LibInterfaces() {
        for (auto& lib_interface : interfaces) {
            std::cout << "Releasing library: " << lib_interface->getCommandName() << std::endl;
        }

        std::cout << "All libraries have been released." << std::endl;
    }

private:
    std::list<std::unique_ptr<LibInterface>> interfaces;  ///< Collection of loaded LibInterface objects
};

#endif

