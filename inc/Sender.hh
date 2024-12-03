#ifndef SENDER_HH
#define SENDER_HH

#include <string>
#include <mutex>
#include <atomic>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>     // For sockaddr_in
#include <sys/socket.h>     // For socket operations
#include <arpa/inet.h>      // For inet_addr
#include "AbstractComChannel.hh"

/*!
 * \brief Handles communication with the graphical server.
 *
 * This class provides functionality to send preformatted commands
 * to the graphical server using a network connection.
 */
class Sender : public AbstractComChannel {
private:
    int Socket;                        // Network socket descriptor
    std::mutex Mutex;                  // Mutex for thread safety
    std::atomic<bool> Connected;       // Indicates whether the connection is active

    /*!
     * \brief Sends a string message through the socket.
     * \param message The message to send.
     */
    void Send(const std::string& message) {
        std::lock_guard<std::mutex> lock(Mutex);
        ssize_t totalSent = 0;
        ssize_t toSend = message.size();
        const char* data = message.c_str();

        while (toSend > 0) {
            ssize_t sent = write(Socket, data + totalSent, toSend);
            if (sent < 0) {
                std::cerr << "*** Error sending message: " << strerror(errno) << std::endl;
                Connected = false;
                return;
            }
            totalSent += sent;
            toSend -= sent;
        }
    }

public:
    /*!
     * \brief Constructor.
     */
    Sender() : Socket(0), Connected(false) {}

    /*!
     * \brief Establishes a connection to the server.
     * \param ipAddress The server's IP address.
     * \param port The server's port.
     * \return True if the connection was successful, false otherwise.
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

        Connected = true;
        return true;
    }

    /*!
     * \brief Sends a preformatted command to the server.
     * \param command The command string to send.
     */
    void SendCommand(const std::string& command) {
        if (!Connected) {
            std::cerr << "Sender is not connected to the server.\n";
            return;
        }
        Send(command);
    }

    /*!
     * \brief Implements AbstractComChannel::Init.
     */
    void Init(int socket) override {
        Socket = socket;
        Connected = true;
    }

    /*!
     * \brief Implements AbstractComChannel::GetSocket.
     */
    int GetSocket() const override {
        return Socket;
    }

    /*!
     * \brief Implements AbstractComChannel::LockAccess.
     */
    void LockAccess() override {
        Mutex.lock();
    }

    /*!
     * \brief Implements AbstractComChannel::UnlockAccess.
     */
    void UnlockAccess() override {
        Mutex.unlock();
    }

    /*!
     * \brief Implements AbstractComChannel::UseGuard.
     */
    std::mutex& UseGuard() override {
        return Mutex;
    }

    /*!
     * \brief Destructor.
     */
    ~Sender() {
        if (Socket >= 0) {
            close(Socket);
        }
    }
};

#endif