#include "ProgramInterpreter.hh"
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <sstream>
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

        auto command = libInterface->getCommandInstance();
        if (!command->ReadParams(stream)) {
            std::cerr << "Error reading parameters for command: " << cmdName << std::endl;
            return false;
        }

        command->PrintCmd();

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


bool ProgramInterpreter::LoadLibraries() {
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

    // // Example: Iterate through Scene objects and perform actions
    // for (const auto& mobileObj : config.GetMobileObjects()) {
    //     auto* obj = scene.FindMobileObj(mobileObj->GetName().c_str());
    //     if (obj) {
    //         std::cout << "Executing logic for object: " << obj->GetName() << std::endl;
    //         // Perform actions with obj
    //     }
    // }
}
