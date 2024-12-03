#ifndef CUBOID_HH
#define CUBOID_HH

#include "AbstractMobileObj.hh"
#include "Vector3D.hh"
#include <string>

/*!
 * \class Cuboid
 * \brief Represents a cuboid object in the scene.
 *
 * Implements the AbstractMobileObj interface.
 */
class Cuboid : public AbstractMobileObj {
private:
    std::string name;   //!< Name of the cuboid
    Vector3D position;  //!< Position in meters
    Vector3D scale;     //!< Scale factors
    Vector3D rotation;  //!< Rotation angles (roll, pitch, yaw) in degrees
    Vector3D rgb;       //!< RGB color

public:
    Cuboid(const std::string& name, const Vector3D& position, const Vector3D& scale,
           const Vector3D& rotation, const Vector3D& rgb)
        : name(name), position(position), scale(scale), rotation(rotation), rgb(rgb) {}

    virtual ~Cuboid() override = default;

    // Getters
    virtual double GetAng_Roll_deg() const override { return rotation[0]; }
    virtual double GetAng_Pitch_deg() const override { return rotation[1]; }
    virtual double GetAng_Yaw_deg() const override { return rotation[2]; }

    virtual const Vector3D& GetPositoin_m() const override { return position; }
    virtual const std::string& GetName() const override { return name; }

    // Setters
    virtual void SetAng_Roll_deg(double Ang_Roll_deg) override { rotation[0] = Ang_Roll_deg; }
    virtual void SetAng_Pitch_deg(double Ang_Pitch_deg) override { rotation[1] = Ang_Pitch_deg; }
    virtual void SetAng_Yaw_deg(double Ang_Yaw_deg) override { rotation[2] = Ang_Yaw_deg; }

    virtual void SetPosition_m(const Vector3D& rPos) override { position = rPos; }
    virtual void SetName(const char* sName) override { name = sName; }

    // Additional getters for scale and color
    const Vector3D& GetScale() const { return scale; }
    const Vector3D& GetColor() const { return rgb; }
};

#endif
