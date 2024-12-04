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
bool Interp4Rotate::ExecCmd(AbstractScene &rScn, const char *sMobObjName, AbstractComChannel &rComChann) {
    std::cout << "ExecCmd Interp4Rotate for rotation" << std::endl;

    AbstractMobileObj* obj = rScn.FindMobileObj(Object_name.c_str());
    if (!obj) {
        std::cerr << "Object not found: " << sMobObjName << std::endl;
        return false;
    }

    const int fps = 30;
    const double frameTimeMs = 1000.0 / fps;

    double durationMs = (Angle / Angle_speed) * 1000.0;
    int totalFrames = static_cast<int>(std::ceil(durationMs / frameTimeMs));

    double stepAngle = Angle / totalFrames;

    for (int frame = 0; frame < totalFrames; ++frame) {
        {
            std::lock_guard<std::mutex> lock(rScn.GetMutex());

            if (Axis_name == "OX") {
                obj->SetAng_Roll_deg(obj->GetAng_Roll_deg() + stepAngle);
            } else if (Axis_name == "OY") {
                obj->SetAng_Pitch_deg(obj->GetAng_Pitch_deg() + stepAngle);
            } else if (Axis_name == "OZ") {
                obj->SetAng_Yaw_deg(obj->GetAng_Yaw_deg() + stepAngle);
            } else {
                std::cerr << "Unknown axis: " << Axis_name << std::endl;
                return false;
            }
        }

        std::ostringstream commandStream;
        commandStream << "UpdateObj Name=" << Object_name.c_str()
                      << " RotXYZ_deg=(" << obj->GetAng_Roll_deg() << ","
                      << obj->GetAng_Pitch_deg() << "," << obj->GetAng_Yaw_deg() << ")\n";

        std::string command = commandStream.str();

        rComChann.LockAccess();
        try {
            dynamic_cast<Sender&>(rComChann).SendCommand(command);
        } catch (...) {
            rComChann.UnlockAccess();
            throw;
        }
        rComChann.UnlockAccess();

        usleep(static_cast<int>(frameTimeMs * 1000));
    }

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
