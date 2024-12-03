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

    // Load configuration file
    // Configuration config;
    // if (!ReadFile("config/config.xml", config)) return 1;

    // Preprocess command file
    // istringstream commandStream;
    // if (!RunPreprocessor("cmd_list.txt", commandStream)) {
    //     cerr << "Error running preprocessor.\n";

    //     return 1;
    // }

    // // Set up plugins with Set4LibInterfaces
    // Set4LibInterfaces pluginManager;
    // if (!pluginManager.addLibrary("libInterp4Set.so", "Set") ||
    //     !pluginManager.addLibrary("libInterp4Move.so", "Move") ||
    //     !pluginManager.addLibrary("libInterp4Rotate.so", "Rotate") ||
    //     !pluginManager.addLibrary("libInterp4Pause.so", "Pause")) {
    //     cerr << "Error loading libraries.\n";
    //     return 1;
    // }

    // Set up plugins using the configuration
    // Set4LibInterfaces pluginManager;
    // const auto& libs = config.GetLibs();
    // config.Print();
    // //std::cout << std::flush;

    // for (const auto& lib : libs) {
    //     //std::cout << "Lib name " << lib << "\n";

    //     // Use the library name as both the file name and the identifier (if no specific identifier is needed)
    //     std::string identifier = lib.substr(lib.find_last_of('/') + 1); // Extract filename as identifier
    //     identifier = identifier.substr(0, identifier.find_last_of('.'));

    //     //std::cout << "Lib identifier " << identifier << "\n";

    //     if (!pluginManager.addLibrary(lib, identifier)) {
    //         std::cerr << "Error loading library: " << lib << "\n";
    //         return 1;
    //     }
    // }

    // Sender sender(config.GetCubes());
    // if (!sender.Connect("127.0.0.1", 6217)) {
    //     return 1;
    // }

    // Start communication in a separate thread (sleep)
    // sender.Start();

    // Simulate additional logic or wait for completion
    // usleep(2000000);

    // Stop communication
    // sender.Stop();

    // Read and execute commands from commandStream # TODO: READ CMD FROM CONF
    // string commandName;
    // while (commandStream >> commandName) {
    //     // Find the matching plugin for the command
    //     LibInterface* commandInterface = pluginManager.getInterface(commandName);
    //     if (!commandInterface) {
    //         cerr << "Unknown command: " << commandName << endl;
    //         return 2;
    //     }

    //     AbstractInterp4Command* command = commandInterface->getCommandInstance();
    //     if (!command) {
    //         cerr << "Failed to retrieve command instance for: " << commandName << endl;
    //         return 2;
    //     }

    //     // Read and execute the command
    //     command->ReadParams(commandStream);
    //     command->PrintCmd();
    // }

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