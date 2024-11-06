#include <iostream>
#include <dlfcn.h>
#include <sstream>
#include <list>
#include <string>
#include <memory>
#include <vector> 

#include "AbstractInterp4Command.hh"
#include "xmlinterp.hh"
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

#define LINE_SIZE 500

using namespace std;
using namespace xercesc;

bool InitializeXMLParser() {

    try {
        XMLPlatformUtils::Initialize();

    } catch (const XMLException& ex) {
        char* message = XMLString::transcode(ex.getMessage());
        cerr << "Error during initialization: " << message << "\n";
        XMLString::release(&message);

        return false;
    }

    return true;
}

bool LoadGrammar(SAX2XMLReader* parser, const char* schemaFile) {
    if (!parser->loadGrammar(schemaFile, Grammar::SchemaGrammarType, true)) {
        cerr << "Failed to load grammar from: " << schemaFile << endl;

        return false;
    }

    parser->setFeature(XMLUni::fgXercesUseCachedGrammarInParse, true);

    return true;
}

bool ReadFile(const char* fileName, Configuration& config) {
    if (!InitializeXMLParser()) return false;

    unique_ptr<SAX2XMLReader> parser(XMLReaderFactory::createXMLReader());
    parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);
    parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
    parser->setFeature(XMLUni::fgXercesSchema, true);
    parser->setFeature(XMLUni::fgXercesValidationErrorAsFatal, true);

    unique_ptr<DefaultHandler> handler(new XMLInterp4Config(config));
    parser->setContentHandler(handler.get());
    parser->setErrorHandler(handler.get());

    try {
        if (!LoadGrammar(parser.get(), "config/config.xsd")) return false;
        parser->parse(fileName);

    } catch (const XMLException& ex) {
        char* message = XMLString::transcode(ex.getMessage());
        cerr << "XML Exception: " << message << "\n";
        XMLString::release(&message);
        return false;
        
    } catch (const SAXParseException& ex) {
        char* message = XMLString::transcode(ex.getMessage());
        cerr << "Parse Error at line " << ex.getLineNumber() << ", column " << ex.getColumnNumber() << ": " << message << "\n";
        XMLString::release(&message);
        return false;
        
    } catch (...) {
        cerr << "Unexpected exception occurred.\n";

        return false;
    }

    return true;
}

bool RunPreprocessor(const char* fileName, istringstream& inputStream) {
    string command = "cpp -P " + string(fileName);
    char line[LINE_SIZE];
    ostringstream outputStream;

    FILE* process = popen(command.c_str(), "r");
    if (!process) return false;

    while (fgets(line, LINE_SIZE, process)) {
        outputStream << line;
    }

    inputStream.str(outputStream.str());

    return pclose(process) == 0;
}

AbstractInterp4Command* LoadPlugin(const char* libraryName) {
    void* libraryHandle = dlopen(libraryName, RTLD_LAZY);
    if (!libraryHandle) {
        cerr << "Library not found: " << libraryName << endl;
        return nullptr;
    }

    void* createFunc = dlsym(libraryHandle, "CreateCmd");
    if (!createFunc) {
        cerr << "Function CreateCmd not found in " << libraryName << endl;
        dlclose(libraryHandle);
        return nullptr;
    }

    using CreateCmdFunc = AbstractInterp4Command* (*)();
    CreateCmdFunc createCommand = reinterpret_cast<CreateCmdFunc>(createFunc);
    return createCommand();
}

int main() {
    Configuration config;
    if (!ReadFile("config/config.xml", config)) return 1;

    istringstream commandStream;
    if (!RunPreprocessor("cmd_list.txt", commandStream)) {
        cerr << "Error running preprocessor.\n";
        return 1;
    }

    struct CommandInfo {
        const char* name;
        unique_ptr<AbstractInterp4Command> command;
    };
    
    vector<CommandInfo> commands;
    commands.emplace_back(CommandInfo{"Set", std::move(unique_ptr<AbstractInterp4Command>(LoadPlugin("libInterp4Set.so")))});
    commands.emplace_back(CommandInfo{"Move", std::move(unique_ptr<AbstractInterp4Command>(LoadPlugin("libInterp4Move.so")))});
    commands.emplace_back(CommandInfo{"Rotate", std::move(unique_ptr<AbstractInterp4Command>(LoadPlugin("libInterp4Rotate.so")))});
    commands.emplace_back(CommandInfo{"Pause", std::move(unique_ptr<AbstractInterp4Command>(LoadPlugin("libInterp4Pause.so")))});

    for (const auto& cmdInfo : commands) {
        if (!cmdInfo.command) {
            cerr << "Failed to initialize command: " << cmdInfo.name << endl;
            return 1;
        }
    }

    string commandName;
    while (commandStream >> commandName) {
        bool found = false;

        for (auto& cmdInfo : commands) {
            if (commandName == cmdInfo.name) {
                cmdInfo.command->ReadParams(commandStream);
                cmdInfo.command->PrintCmd();
                found = true;
                break;
            }
        }

        if (!found) {
            cerr << "Unknown command in file: " << commandName << endl;
            return 2;
        }
    }

    return 0;
}
