#include <iostream>
#include <sstream>
#include "Interp4Set.hh"

using std::cout;
using std::endl;

extern "C" {
  AbstractInterp4Command* CreateCmd(void);
  const char* GetCmdName() { return "Set"; }
}

/*!
 * \brief Factory function for creating an Interp4Set command.
 */
AbstractInterp4Command* CreateCmd(void) {
  return Interp4Set::CreateCmd();
}

/*!
 * \brief Default constructor.
 */
Interp4Set::Interp4Set() 
  : Coordinate_X(0), Coordinate_Y(0), Coordinate_Z(0),
    Angle_X(0), Angle_Y(0), Angle_Z(0),
    Scale_X(1), Scale_Y(1), Scale_Z(1), // Default scale to 1
    Color_R(255), Color_G(255), Color_B(255) {} // Default color to white

/*!
 * \brief Print the command for debugging.
 */
void Interp4Set::PrintCmd() const {
  cout << GetCmdName() << ": "  << Object_name << " "
       << Coordinate_X << " " << Coordinate_Y << " " << Coordinate_Z << " "
       << Angle_X << " " << Angle_Y << " " << Angle_Z << " "
       << Scale_X << " " << Scale_Y << " " << Scale_Z << " "
       << Color_R << " " << Color_G << " " << Color_B << endl;
}

/*!
 * \brief Return the command name.
 */
const char* Interp4Set::GetCmdName() const {
  return ::GetCmdName();
}

/*!
 * \brief Execute the command by sending it to the graphical server.
 */
bool Interp4Set::ExecCmd(AbstractScene &rScn, const char *sMobObjName, AbstractComChannel &rComChann) {
  	std::cout << "ExecCmd Interp4Set" << std::endl;
    AbstractMobileObj* obj = rScn.FindMobileObj(Object_name.c_str());

    if (!obj) {
        std::cerr << "Object not found: " << Object_name << std::endl;
        return false;
    }

    std::ostringstream commandStream;
    commandStream << "AddObj Name=" << Object_name
                  << " Scale=(" << Scale_X << "," << Scale_Y << "," << Scale_Z << ")"
                  << " Shift=(" << Coordinate_X << "," << Coordinate_Y << "," << Coordinate_Z << ")"
                  << " RotXYZ_deg=(" << Angle_X << "," << Angle_Y << "," << Angle_Z << ")"
                  << " RGB=(" << Color_R << "," << Color_G << "," << Color_B << ")\n";

    std::string command = commandStream.str();

    // Lock access to the channel while sending the command
    rComChann.LockAccess();
    try {
		std::cout << "Before SendCommand" << std::endl;
        dynamic_cast<Sender&>(rComChann).SendCommand(command);
    } catch (...) {
		std::cerr << "Exception occurred while sending command." << std::endl;
        rComChann.UnlockAccess();
        throw;
    }

	std::cout << "Before UnlockAccess" << std::endl;
    rComChann.UnlockAccess();

    return true;
}

/*!
 * \brief Read parameters for the command.
 */
bool Interp4Set::ReadParams(std::istream& Strm_CmdsList) {
  Strm_CmdsList >> Object_name 
                >> Coordinate_X >> Coordinate_Y >> Coordinate_Z
                >> Angle_X >> Angle_Y >> Angle_Z
                >> Scale_X >> Scale_Y >> Scale_Z
                >> Color_R >> Color_G >> Color_B;
  return true;
}

/*!
 * \brief Factory method for creating an Interp4Set command.
 */
AbstractInterp4Command* Interp4Set::CreateCmd() {
  return new Interp4Set();
}

/*!
 * \brief Print syntax for the command.
 */
void Interp4Set::PrintSyntax() const {
  cout << "   Set <ObjectName> <X[m]> <Y[m]> <Z[m]> <AngleX[deg]> <AngleY[deg]> <AngleZ[deg]> "
       << "<ScaleX> <ScaleY> <ScaleZ> <R> <G> <B>" << endl;
}

