#include <iostream>
#include "Interp4Rotate.hh"


using std::cout;
using std::endl;


extern "C" {
  AbstractInterp4Command* CreateCmd(void);
  const char* GetCmdName() { return "Rotate"; }
}




/*!
 * \brief
 *
 *
 */
AbstractInterp4Command* CreateCmd(void)
{
  return Interp4Rotate::CreateCmd();
}


/*!
 *
 */
Interp4Rotate::Interp4Rotate(): Axis_name("OZ"), Angle_speed(0), Angle(0)
{}


/*!
 *
 */
void Interp4Rotate::PrintCmd() const
{
  cout << GetCmdName() << ": " << Object_name << " " << Axis_name << " "
       << Angle_speed << " " << Angle << endl;
}


/*!
 *
 */
const char* Interp4Rotate::GetCmdName() const
{
  return ::GetCmdName();
}


/*!
 *
 */
bool Interp4Rotate::ExecCmd(AbstractScene &rScn, const char *sMobObjName, AbstractComChannel &rComChann)
{
  /*
   *  Tu trzeba napisać odpowiedni kod.
   */
  return true;
}


/*!
 *
 */
bool Interp4Rotate::ReadParams(std::istream& Strm_CmdsList)
{
  Strm_CmdsList >> Object_name >> Axis_name >> Angle_speed >> Angle;
  return true;
}


/*!
 *
 */
AbstractInterp4Command* Interp4Rotate::CreateCmd()
{
  return new Interp4Rotate();
}


/*!
 *
 */
void Interp4Rotate::PrintSyntax() const
{
  cout << "   Rotate NazwaObiektu NazwaOsi SzybkoscKatowa[stopnie/s] KatObrotu[stopnie]" << endl;
}
