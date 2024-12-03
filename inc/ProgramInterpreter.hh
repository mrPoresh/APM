#ifndef PROGRAMINTERPRETER_HH
#define PROGRAMINTERPRETER_HH

#include <string>
#include "AbstractScene.hh"
#include "xmlinterp.hh"
#include "Set4LibInterfaces.hh"
#include "Cuboid.hh"
#include "Scene.hh"
#include "Sender.hh"
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/util/XMLString.hpp>

class ProgramInterpreter {
public:
    ProgramInterpreter() = default;
    ~ProgramInterpreter() = default;

    /*!
     * \brief Initializes the interpreter with configuration and commands.
     * \param[in] configPath - Path to the XML configuration file.
     * \param[in] commandsPath - Path to the commands file.
     * \return True if both configuration and commands are loaded successfully.
     */
    bool Init(const std::string& configPath, const std::string& commandsPath);

    /*!
     * \brief Executes the main logic of the program.
     */
    void Run();

private:
    /*!
     * \brief Parses the configuration XML file.
     * \param[in] configPath - Path to the XML configuration file.
     * \return True if the XML file is parsed successfully.
     */
    bool ParseConfigurationFile(const std::string& configPath);

    /*!
     * \brief Loads commands from the command file.
     * \param[in] commandsPath - Path to the commands file.
     * \return True if commands are loaded successfully.
     */
    bool LoadCommands(const std::string& commandsPath);

    /*!
     * \brief Loads commands from the command file.
     * \param[in] commandsPath - Path to the commands file.
     * \return True if commands are loaded successfully.
     */
    bool LoadLibraries();

    bool LoadObjects();

    /*!
     * \brief Initializes the XML parser.
     * \return True if the XML parser initializes successfully.
     */
    bool InitializeXMLParser();

    Scene scene; //!< Instance of the Scene class.
    Sender sender;
    Configuration config;   //!< Configuration object.
    Set4LibInterfaces plugins;    //!< Keep Lis
};

#endif


