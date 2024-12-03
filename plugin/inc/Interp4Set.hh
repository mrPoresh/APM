#ifndef  COMMAND4SET_HH
#define  COMMAND4SET_HH

#ifndef __GNUG__
# pragma interface
# pragma implementation
#endif

#include "AbstractInterp4Command.hh"
#include "Sender.hh"

#include <string>
#include <sstream>

/*!
 * \class Interp4Set
 * \brief Command for setting an object's position, rotation, scale, and color.
 */
class Interp4Set: public AbstractInterp4Command {
  std::string Object_name;
  double Coordinate_X, Coordinate_Y, Coordinate_Z; // Coordinates
  double Angle_X, Angle_Y, Angle_Z;               // Rotation angles
  double Scale_X, Scale_Y, Scale_Z;               // Scale factors
  int Color_R, Color_G, Color_B;                  // RGB color values

public:
  Interp4Set();  

  virtual void PrintCmd() const override;
  virtual void PrintSyntax() const override;
  virtual void PrintParams() const override {}
  virtual const char* GetCmdName() const override;
  virtual bool ExecCmd(AbstractScene &rScn, const char *sMobObjName, AbstractComChannel &rComChann) override;
  virtual bool ReadParams(std::istream& Strm_CmdsList) override;

  static AbstractInterp4Command* CreateCmd();
};

#endif