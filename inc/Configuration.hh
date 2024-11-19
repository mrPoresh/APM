#ifndef CONFIGURATION_HH
#define CONFIGURATION_HH

#include <string>
#include <vector>
#include "Vector3D.hh"

class CubeConfig {
public:
    std::string Name;
    
    Vector3D Scale;
    Vector3D Shift;
    Vector3D Rotation; // in degrees
    Vector3D Translation; // in meters
    Vector3D RGB;

    CubeConfig() = default;

    CubeConfig(const std::string &name, const Vector3D &scale, const Vector3D &shift,
               const Vector3D &rotation, const Vector3D &translation, const Vector3D &rgb)
        : Name(name), Scale(scale), Shift(shift), Rotation(rotation), Translation(translation), RGB(rgb) {}
};

class Configuration {
private:
    std::vector<std::string> Libs;
    std::vector<CubeConfig> Cubes;

public:
    // Add a library to the list
    void AddLib(const std::string &libName) {
        Libs.push_back(libName);
    }

    // Add a cube configuration to the list
    void AddCube(const CubeConfig &cube) {
        Cubes.push_back(cube);
    }

    // Getters
    const std::vector<std::string>& GetLibs() const {
        return Libs;
    }

    const std::vector<CubeConfig>& GetCubes() const {
        return Cubes;
    }

    // Debugging: Print the configuration
    void Print() const {
        std::cout << "Libraries:\n";
        for (const auto &lib : Libs) {
            std::cout << "  - " << lib << "\n";
        }

        std::cout << "\nCubes:\n";
        for (const auto &cube : Cubes) {
            std::cout << "  Name: " << cube.Name << "\n"
                      << "  Scale: " << cube.Scale << "\n"
                      << "  Shift: " << cube.Shift << "\n"
                      << "  Rotation: " << cube.Rotation << "\n"
                      << "  Translation: " << cube.Translation << "\n"
                      << "  RGB: " << cube.RGB << "\n\n";
        }
    }
};

#endif
