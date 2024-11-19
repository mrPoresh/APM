#include <iostream>
#include <sstream>
#include <memory>

#include "xmlinterp.hh"
#include "Set4LibInterfaces.hh"
#include "AbstractInterp4Command.hh"

#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/XMLString.hpp>

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
        if (!parser->loadGrammar("config/config.xsd", Grammar::SchemaGrammarType, true)) {
            cerr << "Failed to load grammar from config/config.xsd\n";
            return false;
        }

        parser->setFeature(XMLUni::fgXercesUseCachedGrammarInParse, true);
        parser->parse(fileName);

    } catch (const XMLException& ex) {
        char* message = XMLString::transcode(ex.getMessage());
        cerr << "XML Exception: " << message << "\n";
        XMLString::release(&message);

        return false;
    } catch (const SAXParseException& ex) {
        char* message = XMLString::transcode(ex.getMessage());
        cerr << "Parse Error: " << message << "\n";
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
    char line[500];
    ostringstream outputStream;

    FILE* process = popen(command.c_str(), "r");
    if (!process) return false;

    while (fgets(line, sizeof(line), process)) {
        outputStream << line;
    }

    inputStream.str(outputStream.str());
    return pclose(process) == 0;
}

int main() {
    // Load configuration file
    Configuration config;
    if (!ReadFile("config/config.xml", config)) return 1;

    // Preprocess command file
    istringstream commandStream;
    if (!RunPreprocessor("cmd_list.txt", commandStream)) {
        cerr << "Error running preprocessor.\n";

        return 1;
    }

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
    Set4LibInterfaces pluginManager;
    const auto& libs = config.GetLibs();
    config.Print();
    std::cout << std::flush;

    for (const auto& lib : libs) {
        std::cout << "Lib name " << lib << "\n";

        // Use the library name as both the file name and the identifier (if no specific identifier is needed)
        std::string identifier = lib.substr(lib.find_last_of('/') + 1); // Extract filename as identifier
        identifier = identifier.substr(0, identifier.find_last_of('.')); // Remove extension

        std::cout << "Lib identifier " << identifier << "\n";

        if (!pluginManager.addLibrary(lib, identifier)) {
            std::cerr << "Error loading library: " << lib << "\n";
            return 1;
        }
    }

    // Read and execute commands from commandStream # TODO: READ CMD FROM CONF
    string commandName;
    while (commandStream >> commandName) {
        // Find the matching plugin for the command
        LibInterface* commandInterface = pluginManager.getInterface(commandName);
        if (!commandInterface) {
            cerr << "Unknown command: " << commandName << endl;
            return 2;
        }

        AbstractInterp4Command* command = commandInterface->getCommandInstance();
        if (!command) {
            cerr << "Failed to retrieve command instance for: " << commandName << endl;
            return 2;
        }

        // Read and execute the command
        command->ReadParams(commandStream);
        command->PrintCmd();
    }

    cout << "\nProgram End\n\n";

    return 0;
}

