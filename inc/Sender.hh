#ifndef SENDER_HH
#define SENDER_HH

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>     // For sockaddr_in
#include <sys/socket.h>     // For socket operations
#include <arpa/inet.h>      // For inet_addr
#include "Configuration.hh"

class Sender {
private:
    int Socket; // Network socket descriptor
    
    std::atomic<bool> ContinueLooping; // Control flag for the thread
    std::thread CommunicationThread; // Communication thread
    // Mutex => From "Scene"
    
    const std::vector<CubeConfig>& Cubes; // Reference to the list of cubes

    /*!
     * \brief Generates an AddObj command string for the cube.
     */
    std::string GenerateAddObjCommand(const CubeConfig& cube) const {
        std::ostringstream command;

        command << "AddObj Name=" << cube.Name
                << " Scale=(" << cube.Scale[0] << "," << cube.Scale[1] << "," << cube.Scale[2] << ")"
                << " Shift=(" << cube.Shift[0] << "," << cube.Shift[1] << "," << cube.Shift[2] << ")"
                << " RotXYZ_deg=(" << cube.Rotation[0] << "," << cube.Rotation[1] << "," << cube.Rotation[2] << ")"
                << " Trans_m=(" << cube.Translation[0] << "," << cube.Translation[1] << "," << cube.Translation[2] << ")"
                << " RGB=(" << cube.RGB[0] << "," << cube.RGB[1] << "," << cube.RGB[2] << ")\n";

        return command.str();
    }

    /*!
     * \brief Sends a string message through the socket.
     */
    void Send(const std::string& message) {
        // std::lock_guard<std::mutex> lock(Mutex); => lock "Scene Mutex"

        ssize_t totalSent = 0;
        ssize_t toSend = message.size();
        const char* data = message.c_str();

        while (toSend > 0) {
            ssize_t sent = write(Socket, data + totalSent, toSend);
            if (sent < 0) {
                std::cerr << "*** Error sending message: " << strerror(errno) << std::endl;
                return;
            }
            totalSent += sent;
            toSend -= sent;
        }
    }

    /*!
     * \brief Main function for the communication thread.
     */
    void CommunicationLoop() {
        for (const auto& cube : Cubes) {
            if (!ContinueLooping) break;

            std::string command = GenerateAddObjCommand(cube);
            Send(command);
            usleep(100000);///
        }
    }

public:
    /*!
     * \brief Constructor.
     * \param cubes Reference to the list of cube configurations.
     */
    Sender(const std::vector<CubeConfig>& cubes) 
        : Socket(0), ContinueLooping(true), Cubes(cubes) {}

    /*!
     * \brief Establishes a connection to the server.
     */
    bool Connect(const std::string& ipAddress, int port) {
        Socket = socket(AF_INET, SOCK_STREAM, 0);

        if (Socket < 0) {
            std::cerr << "*** Error opening socket.\n";
            return false;
        }

        sockaddr_in serverAddress = {};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        serverAddress.sin_addr.s_addr = inet_addr(ipAddress.c_str());

        if (connect(Socket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
            std::cerr << "*** Unable to connect to server.\n";
            return false;
        }

        return true;
    }

    /*!
     * \brief Starts the communication thread.
     */
    void Start() {
        CommunicationThread = std::thread(&Sender::CommunicationLoop, this);
    }

    /*!
     * \brief Stops the communication thread.
     */
    void Stop() {
        ContinueLooping = false;
        if (CommunicationThread.joinable()) {
            CommunicationThread.join();
        }
    }

    /*!
     * \brief Destructor.
     */
    ~Sender() {
        Stop();
        if (Socket >= 0) {
            close(Socket);
        }
    }
};

#endif
