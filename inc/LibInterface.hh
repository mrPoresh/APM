#ifndef LIBINTERFACE_HH
#define LIBINTERFACE_HH

#include <string>
#include <memory>
#include <dlfcn.h>
#include "AbstractInterp4Command.hh"

/*!
 * \class LibInterface
 * \brief Manages a single library (plugin) and its associated command.
 *
 * This class loads a dynamic library, retrieves a function pointer to create
 * the associated command, and manages the command's instance.
 */
class LibInterface {
public:
    /*!
     * \brief Constructs a LibInterface with the library and command names.
     * \param libName Name of the dynamic library file to load (e.g., "libInterp4Move.so").
     * \param cmdName Name of the command associated with this library.
     */
    LibInterface(const std::string& libName, const std::string& cmdName)
        : libraryName(libName), commandName(cmdName), libraryHandle(nullptr), commandInstance(nullptr) {}

    /*!
     * \brief Loads the dynamic library and creates the command instance.
     * \return True if the library and command instance were loaded successfully, false otherwise.
     */
    bool loadLibrary() {
        libraryHandle = dlopen(libraryName.c_str(), RTLD_LAZY);
        if (!libraryHandle) {
            std::cerr << "Failed to load library: " << libraryName << "\n";
            std::cerr << "dlerror: " << dlerror() << "\n";
            return false;
        }

        auto createCmdFunc = reinterpret_cast<AbstractInterp4Command* (*)()>(dlsym(libraryHandle, "CreateCmd"));
        
        if (!createCmdFunc) {
            std::cerr << "Function CreateCmd not found in " << libraryName << "\n";
            std::cerr << "dlerror: " << dlerror() << "\n";
            dlclose(libraryHandle);
            return false;
        }

        commandInstance = std::unique_ptr<AbstractInterp4Command>(createCmdFunc());
        return true;
    }

    /*!
     * \brief Unloads the dynamic library if it was loaded.
     */
    void unloadLibrary() {
        if (libraryHandle) {
            dlclose(libraryHandle);  // Close the library handle
            std::cout << "LibInterface destructor\n";
            libraryHandle = nullptr;
        }
    }

    /*!
     * \brief Returns the name of the command associated with this library.
     * \return The command name as a const reference to a string.
     */
    const std::string& getCommandName() const { return commandName; }

    /*!
     * \brief Provides access to the command instance created by this library.
     * \return Pointer to the AbstractInterp4Command instance.
     */
    AbstractInterp4Command* getCommandInstance() const { return commandInstance.get(); }

    /*!
     * \brief Destructor to ensure the library is unloaded on destruction.
     */
     ~LibInterface() {
        commandInstance.reset();
        unloadLibrary();
    }

private:
    std::string libraryName;  ///< Name of the dynamic library file
    std::string commandName;  ///< Name of the command associated with this library
    void* libraryHandle;      ///< Handle to the loaded dynamic library
    std::unique_ptr<AbstractInterp4Command> commandInstance;  ///< Instance of the command from the library
};

#endif

