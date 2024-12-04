#ifndef CONFIGURATION_HH
#define CONFIGURATION_HH

#include <string>
#include <list>
#include <map>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include "Vector3D.hh"
#include "AbstractInterp4Command.hh"

/*!
 * \class CubeConfig
 * \brief Represents the configuration of a single cube object in the scene.
 */
class CubeConfig {
public:
    std::string Name;      //!< Name of the cube
    Vector3D Scale;        //!< Scale of the cube
    Vector3D Shift;        //!< Shift of the cube
    Vector3D Rotation;     //!< Rotation angles of the cube (degrees)
    Vector3D Translation;  //!< Translation of the cube
    Vector3D RGB;          //!< RGB color of the cube

    CubeConfig() = default;

    CubeConfig(const std::string& name, const Vector3D& scale, const Vector3D& shift,
               const Vector3D& rotation, const Vector3D& translation, const Vector3D& rgb)
        : Name(name), Scale(scale), Shift(shift), Rotation(rotation), Translation(translation), RGB(rgb) {}
};

/*!
 * \class Configuration
 * \brief Manages libraries, cube configurations, and commands for the scene.
 */
class Configuration {
private:
    std::list<std::string> Libs;                      //!< List of library paths
    std::map<std::string, std::string> LibCommands;   //!< Map of library paths to command names
    std::list<CubeConfig> Cubes;                      //!< List of cube configurations
    std::list<std::list<AbstractInterp4Command*>> Commands; //!< List of commands, including parallel groups
    std::map<std::string, double> Constants;

public:
    /*!
     * \brief Adds a library and maps it to its command name.
     * \param libName The name of the library file.
     */
    void AddLib(const std::string& libName) {
        Libs.push_back(libName);

        // Extract command name by removing 'libInterp4' prefix and '.so' suffix
        std::string commandName = libName.substr(10, libName.find_last_of('.') - 10);
        LibCommands[libName] = commandName;
    }

    /*!
     * \brief Adds a standalone command to the configuration.
     * \param command Pointer to the command to add.
     */
    void AddStandaloneCommand(AbstractInterp4Command* command) {
        Commands.push_back({command}); // Wrap the command in a single-element list
        this->PrintCommands();
    }

    /*!
     * \brief Adds a list of parallel commands to the configuration.
     * \param parallelCommands List of commands to add as a parallel group.
     */
    void AddParallelCommands(const std::list<AbstractInterp4Command*>& parallelCommands) {
        Commands.push_back(parallelCommands);
    }

    /*!
     * \brief Adds a cube configuration to the list.
     * \param cube The CubeConfig object to add.
     */
    void AddCube(const CubeConfig& cube) {
        Cubes.push_back(cube);
    }

    /*!
     * \brief Retrieves the list of libraries.
     * \return A reference to the list of library paths.
     */
    const std::list<std::string>& GetLibs() const {
        return Libs;
    }

    /*!
     * \brief Retrieves the command name associated with a library.
     * \param libName The library name.
     * \return The command name associated with the library.
     * \throws std::runtime_error If the library is not found.
     */
    const std::string& GetCommandName(const std::string& libName) const {
        auto it = LibCommands.find(libName);
        if (it != LibCommands.end()) {
            return it->second;
        }
        throw std::runtime_error("Library not found: " + libName);
    }

    /*!
     * \brief Retrieves the list of cube configurations.
     * \return A reference to the list of cube configurations.
     */
    const std::list<CubeConfig>& GetCubes() const {
        return Cubes;
    }

    /*!
     * \brief Retrieves the list of commands.
     * \return A reference to the list of command groups.
     */
    const std::list<std::list<AbstractInterp4Command*>>& GetCommands() const {
        return Commands;
    }

    void AddConstant(const std::string& name, double value) {
        Constants[name] = value;
    }

    const std::map<std::string, double>& GetConstants() const {
        return Constants;
    }

    double GetConstantValue(const std::string& name) const {
        auto it = Constants.find(name);
        if (it != Constants.end()) {
            return it->second;
        }
        throw std::runtime_error("Constant not found: " + name);
    }

    std::string SubstituteConstants(const std::string& commandLine) const {
        std::istringstream iss(commandLine);
        std::ostringstream oss;
        std::string token;

        while (iss >> token) {
            if (Constants.count(token)) {
                oss << Constants.at(token) << " ";
            } else {
                oss << token << " ";
            }
        }
        return oss.str();
    }

    /*!
     * \brief Prints the loaded libraries, commands, and cube configurations.
     */
    void Print() const {
        std::cout << "* Libraries and Commands: *\n";
        for (const auto& lib : LibCommands) {
            std::cout << "  Library: " << lib.first << ", Command: " << lib.second << "\n";
        }

        std::cout << "\nCubes:\n";
        for (const auto& cube : Cubes) {
            std::cout << "  Name: " << cube.Name << "\n"
                      << "  Scale: " << cube.Scale << "\n"
                      << "  Shift: " << cube.Shift << "\n"
                      << "  Rotation: " << cube.Rotation << "\n"
                      << "  Translation: " << cube.Translation << "\n"
                      << "  RGB: " << cube.RGB << "\n";
        }

        std::cout << "* ------------------------------ *\n";
    }

    /*!
     * \brief Prints all commands, including parallel groups.
     */
    void PrintCommands() const {
        std::cout << "* All Commands: *\n";
        for (const auto& commandGroup : Commands) {
            if (commandGroup.size() > 1) {
                std::cout << "Parallel Commands Start:\n";
                for (const auto& command : commandGroup) {
                    command->PrintCmd();
                }
                std::cout << "Parallel Commands End\n";
            } else {
                commandGroup.front()->PrintCmd();
            }
        }

        std::cout << "* ------------------------------ *\n";
    }
};

#endif

