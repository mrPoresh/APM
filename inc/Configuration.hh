#ifndef CONFIGURATION_HH
#define CONFIGURATION_HH

#include <string>
#include <vector>
#include <list>
#include <map>
#include <memory>

#include "Vector3D.hh"
#include "AbstractInterp4Command.hh"

class CubeConfig {
public:
    std::string Name;
    
    Vector3D Scale;
    Vector3D Shift;
    Vector3D Rotation;
    Vector3D Translation;
    Vector3D RGB;

    CubeConfig() = default;

    CubeConfig(const std::string &name, const Vector3D &scale, const Vector3D &shift,
               const Vector3D &rotation, const Vector3D &translation, const Vector3D &rgb)
        : Name(name), Scale(scale), Shift(shift), Rotation(rotation), Translation(translation), RGB(rgb) {}
};

// Add a map for library-command relationships
class Configuration {
private:
    std::list<std::string> Libs;                      // List of libraries
    std::map<std::string, std::string> LibCommands;   // Map of lib paths to command names
    std::list<CubeConfig> Cubes;                      // List of cube configurations
    std::list<std::list<AbstractInterp4Command*>> Commands;

public:
    // Add a library and automatically map its command name
    void AddLib(const std::string &libName) {
        Libs.push_back(libName);

        // Extract command name from library name
        std::string commandName = libName.substr(10, libName.find_last_of('.') - 10); // Remove 'libInterp4' and '.so'
        LibCommands[libName] = commandName;
    }

    // Add a standalone command (single-element list)
    void AddStandaloneCommand(AbstractInterp4Command* command) {
        Commands.push_back({command});
    }

    // Add a list of parallel commands
    void AddParallelCommands(const std::list<AbstractInterp4Command*>& parallelCommands) {
        Commands.push_back(parallelCommands);
    }

    // Get the list of libraries
    const std::list<std::string>& GetLibs() const {
        return Libs;
    }

    // Get the command name for a library
    const std::string& GetCommandName(const std::string& libName) const {
        auto it = LibCommands.find(libName);
        if (it != LibCommands.end()) {
            return it->second;
        }
        throw std::runtime_error("Library not found: " + libName);
    }

    const std::list<CubeConfig>& GetCubes() const {
        return Cubes;
    }

    const std::list<std::list<AbstractInterp4Command*>>& GetCommands() const {
        return Commands;
    }

    void Print() const {
        std::cout << "* Libraries and Commands: *\n";
        for (const auto &lib : LibCommands) {
            std::cout << "  Library: " << lib.first << ", Command: " << lib.second << "\n";
        }

        std::cout << "\nCubes:\n";
        for (const auto &cube : Cubes) {
            std::cout << "  Name: " << cube.Name << "\n"
                      << "  Scale: " << cube.Scale << "\n"
                      << "  Shift: " << cube.Shift << "\n"
                      << "  Rotation: " << cube.Rotation << "\n"
                      << "  Translation: " << cube.Translation << "\n"
                      << "  RGB: " << cube.RGB << "\n";
        }

        std::cout << "* ------------------------------ *\n";
    }

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
