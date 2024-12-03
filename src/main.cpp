#include <iostream>
#include <sstream>
#include <memory>

#include "xmlinterp.hh"
#include "Set4LibInterfaces.hh"
#include "AbstractInterp4Command.hh"
#include "Sender.hh"
#include "ProgramInterpreter.hh"

#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/XMLString.hpp>

using namespace std;
using namespace xercesc;


int main(int argc, char* argv[]) {
    if (argc < 3 || argc > 3) {
        std::cerr << "Usage: " << argv[0] << " <config.xml> <commands.txt>" << std::endl;
        return 1;
    }

    const std::string configPath = argv[1];
    const std::string commandsPath = argv[2];

    ProgramInterpreter interpreter;
    if (!interpreter.Init(configPath, commandsPath)) {
        return 1;
    }

    interpreter.Run();

    cout << "\nProgram End\n\n";

    return 0;
}






/*



|               1) Read conf.xml => parse conf => get obj data => set as vector
|   Init Scene  2) Grab vectors => create obs. Object classes for each obj => add pointer to Obs. Scene class
|               3) notes: Uniq obj name, check colision?, Scene must keep own mutex.

|               |
|   Main loop   |   set file name => line inside osstream for each line => 
|               |       is Being Parralel == Yes => skip line, set parallel flag => for each next => | 
|                                         == No                                                   => | create thread ((load comnd from plug. manager).execCmd(), scene, chanel)
|                                                                                                    |
|
|                                                                                                    => thread join
*/