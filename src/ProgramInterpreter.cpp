#include "ProgramInterpreter.hh"
#include <iostream>
#include <fstream>
#include <memory>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/XMLString.hpp>

using namespace xercesc;

bool ProgramInterpreter::Init(const std::string& configPath, const std::string& commandsPath) {
    if (!ParseConfigurationFile(configPath)) {
        std::cerr << "Failed to load configuration from: " << configPath << std::endl;
        return false;
    }

    if (!LoadCommands(commandsPath)) {
        std::cerr << "Failed to load commands from: " << commandsPath << std::endl;
        return false;
    }

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

    std::string command;
    while (std::getline(file, command)) {
        // Handle Comand
        std::cout << "Loaded command: " << command << std::endl;
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
