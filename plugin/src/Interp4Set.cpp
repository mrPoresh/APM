#include <iostream>
#include "Interp4Set.hh"


using std::cout;
using std::endl;


extern "C" {
  AbstractInterp4Command* CreateCmd(void);
  const char* GetCmdName() { return "Set"; }
}




/*!
 * \brief
 *
 *
 */
AbstractInterp4Command* CreateCmd(void)
{
  return Interp4Set::CreateCmd();
}


/*!
 *
 */
Interp4Set::Interp4Set(): Coordinate_X(0), Coordinate_Y(0), Coordinate_Z(0), 
Angle_X(0), Angle_Y(0), Angle_Z(0)
{}


/*!
 *
 */
void Interp4Set::PrintCmd() const
{
  cout << GetCmdName() << ": "  << Object_name << " " 
       << Coordinate_X << " " << Coordinate_Y << " " << Coordinate_Z << " "
       << Angle_X << " " << Angle_Y << " " << Angle_Z << endl;
}


/*!
 *
 */
const char* Interp4Set::GetCmdName() const
{
  return ::GetCmdName();
}


/*!
 *
 */
bool Interp4Set::ExecCmd(AbstractScene &rScn, const char *sMobObjName, AbstractComChannel &rComChann)
{
  /*
   *  Tu trzeba napisać odpowiedni kod.
   */
  return true;
}


/*!
 *
 */
bool Interp4Set::ReadParams(std::istream& Strm_CmdsList)
{
  Strm_CmdsList >> Object_name >> Coordinate_X >> Coordinate_Y >> Coordinate_Z
  		>> Angle_X >> Angle_Y >> Angle_Z;
  return true;
}


/*!
 *
 */
AbstractInterp4Command* Interp4Set::CreateCmd()
{
  return new Interp4Set();
}


/*!
 *
 */
void Interp4Set::PrintSyntax() const
{
  cout << "   Set NazwaObiektu WspX[m] WspY[m] WspZ[m] KatX[stopnie] KatY[stopnie] KatZ[stopnie]" 
       << endl;
}
