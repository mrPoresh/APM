#include "ProgramInterpreter.hh"
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <sstream>
#include <thread>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/XMLString.hpp>

using namespace xercesc;

bool ProgramInterpreter::Init(const std::string& configPath, const std::string& commandsPath) {
    if (!ParseConfigurationFile(configPath)) {
        std::cerr << "Failed to load configuration from: " << configPath << std::endl;
        return false;
    }

    if (!LoadLibraries()) {
        std::cerr << "Failed to load libraries from configuration." << std::endl;
        return false;
    }

    if (!LoadObjects()) {
        std::cerr << "Failed to load objects into the scene." << std::endl;
        return false;
    }

    if (!LoadCommands(commandsPath)) {
        std::cerr << "Failed to load commands from: " << commandsPath << std::endl;
        return false;
    }

    std::cout << "Initialization successful!" << std::endl;

    config.Print();
    config.PrintCommands();

    return true;
}

bool ProgramInterpreter::ParseConfigurationFile(const std::string& configPath) {
    if (!InitializeXMLParser()) return false;

    try {
        std::unique_ptr<SAX2XMLReader> parser(XMLReaderFactory::createXMLReader());
        parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);
        parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
        parser->setFeature(XMLUni::fgXercesSchema, true);
        parser->setFeature(XMLUni::fgXercesValidationErrorAsFatal, true);

        std::unique_ptr<DefaultHandler> handler(new XMLInterp4Config(config));
        parser->setContentHandler(handler.get());
        parser->setErrorHandler(handler.get());

        if (!parser->loadGrammar("config/config.xsd", Grammar::SchemaGrammarType, true)) {
            std::cerr << "Failed to load grammar from config/config.xsd\n";
            return false;
        }

        parser->setFeature(XMLUni::fgXercesUseCachedGrammarInParse, true);
        parser->parse(configPath.c_str());

    } catch (const XMLException& ex) {
        char* message = XMLString::transcode(ex.getMessage());
        std::cerr << "XML Exception: " << message << "\n";
        XMLString::release(&message);
        return false;

    } catch (const SAXParseException& ex) {
        char* message = XMLString::transcode(ex.getMessage());
        std::cerr << "Parse Error: " << message << "\n";
        XMLString::release(&message);
        return false;

    } catch (...) {
        std::cerr << "Unexpected exception occurred.\n";
        return false;
        
    }

    return true;
}

bool ProgramInterpreter::InitializeXMLParser() {
    try {
        XMLPlatformUtils::Initialize();
    } catch (const XMLException& ex) {
        char* message = XMLString::transcode(ex.getMessage());
        std::cerr << "Error during XML parser initialization: " << message << "\n";
        XMLString::release(&message);
        return false;
    }

    return true;
}

bool ProgramInterpreter::LoadCommands(const std::string& commandsPath) {
    std::cout << "Loading comands..." << std::endl;
    std::ifstream file(commandsPath);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open commands file: " << commandsPath << std::endl;
        return false;
    }

    std::list<AbstractInterp4Command*> parallelCommands; // Temporary for parallel commands
    bool inParallelBlock = false;

    std::string line;
    while (std::getline(file, line)) {
        // Ignore empty lines or comments
        if (line.empty() || line[0] == '#' || line.substr(0, 2) == "//") {
            continue;
        }

        // Detect ParallelStart and ParallelEnd
        if (line == "ParalelStart") {
            if (inParallelBlock) {
                std::cerr << "Error: Nested ParalelStart detected" << std::endl;
                return false;
            }
            inParallelBlock = true;
            parallelCommands.clear();
            continue;
        }

        if (line == "ParalelEnd") {
            if (!inParallelBlock) {
                std::cerr << "Error: ParalelEnd without matching ParalelStart" << std::endl;
                return false;
            }

            config.AddParallelCommands(parallelCommands);
            inParallelBlock = false;
            continue;
        }

        //Parse command
        std::istringstream stream(line);
        std::string cmdName;
        stream >> cmdName;

        //std::cout << cmdName << "\n";

        LibInterface* libInterface = plugins.getInterface(cmdName);
        if (!libInterface) {
            std::cerr << "Error: Command not found in plugins: " << cmdName << std::endl;
            return false;
        }

        auto command = libInterface->CreateCmd();
        if (!command->ReadParams(stream)) {
            std::cerr << "Error reading parameters for command: " << cmdName << std::endl;
            return false;
        }

        //command->PrintCmd();

        // Add the command to the appropriate list
        if (inParallelBlock) {
            parallelCommands.push_back(command);
        } else {
            config.AddStandaloneCommand(command);
        }
    }

    if (inParallelBlock) {
        std::cerr << "Error: Unclosed ParalelStart block" << std::endl;
        return false;
    }

    return true;
}

bool ProgramInterpreter::LoadObjects() {
    std::cout << "Loading objects into the scene..." << std::endl;

    for (const auto& cubeConfig : config.GetCubes()) {
        // Create a new Cuboid object for the scene
        auto* cuboid = new Cuboid(
            cubeConfig.Name,
            cubeConfig.Translation,
            cubeConfig.Scale,
            cubeConfig.Rotation,
            cubeConfig.RGB
        );

        scene.AddMobileObj(cuboid);
        std::cout << "Added object: " << cuboid->GetName() << std::endl;

        // Create a new instance of the 'Set' command for this object
        LibInterface* libInterface = plugins.getInterface("Set");
        if (!libInterface) {
            std::cerr << "Error: Command 'Set' not found in plugins." << std::endl;
            return false;
        }

        AbstractInterp4Command* setCommand = libInterface->CreateCmd();
        if (!setCommand) {
            std::cerr << "Error: Unable to create 'Set' command instance." << std::endl;
            return false;
        }

        std::istringstream stream(
            cubeConfig.Name + " " +
            std::to_string(cubeConfig.Translation[0]) + " " +
            std::to_string(cubeConfig.Translation[1]) + " " +
            std::to_string(cubeConfig.Translation[2]) + " " +
            std::to_string(cubeConfig.Rotation[0]) + " " +
            std::to_string(cubeConfig.Rotation[1]) + " " +
            std::to_string(cubeConfig.Rotation[2]) + " " +
            std::to_string(cubeConfig.Scale[0]) + " " +
            std::to_string(cubeConfig.Scale[1]) + " " +
            std::to_string(cubeConfig.Scale[2]) + " " +
            std::to_string(cubeConfig.RGB[0]) + " " +
            std::to_string(cubeConfig.RGB[1]) + " " +
            std::to_string(cubeConfig.RGB[2])
        );

        if (!setCommand->ReadParams(stream)) {
            std::cerr << "Error: Unable to read parameters for 'Set' command." << std::endl;
            delete setCommand;
            return false;
        }

        config.AddStandaloneCommand(setCommand);
    }

    return true;
}

bool ProgramInterpreter::LoadLibraries() {
    std::cout << "Loading Libs..." << std::endl;
    const auto& libs = config.GetLibs();
    for (const auto& libName : libs) {
        std::string libPath = "libs/" + libName;
        std::string commandName = config.GetCommandName(libName);

        if (!plugins.addLibrary(libPath, commandName)) {
            std::cerr << "Error loading library: " << libPath << "\n";
            return false;
        }
    }

    return true;
}

void ProgramInterpreter::Run() {
    std::cout << "Running program..." << std::endl;

    if (!sender.Connect("127.0.0.1", 6217)) {
        std::cerr << "Failed to connect to graphical server." << std::endl;
        return;
    }

    std::cout << "Connection OK..." << std::endl;

    const auto& commands = config.GetCommands();
    std::list<std::thread> threads;

    for (const auto& commandGroup : commands) {
        for (auto* command : commandGroup) {
            std::cout << "New command" << std::endl;
            command->PrintCmd();

            threads.emplace_back([command, this]() {
                command->ExecCmd(scene, command->GetCmdName(), sender);
            });
        }

        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        threads.clear();
    }

    std::cout << "Program finished executing commands." << std::endl;
}
