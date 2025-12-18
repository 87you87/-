#pragma once

#include <winsock2.h>
#include <string>

#include "connectUI.h"

#define WM_APP_TCP_MSG (WM_APP + 1)
#define WM_APP_TCP_ERROR (WM_APP + 2)



using std::string;

class Connect_Client{
    SOCKET connectSocket = INVALID_SOCKET;
    sockaddr_in serverAddr;
public:
    Connect_Client();
    ~Connect_Client();

    //bool initializeWSAD();
    //bool initializeSocket();
    void initializeServerAddr(const char*, int);

    bool connectToServer();

    void sendData(const char*);
    string receiveData();
};

class Connect_Server{
    SOCKET listenSocket = INVALID_SOCKET;
    SOCKET clientSocket = INVALID_SOCKET;
    sockaddr_in serverAddr;
    sockaddr_in clientAddr;
    

public:
    Connect_Server();
    ~Connect_Server();

    //bool initializeWSAD();
    //bool initializeSocket();

    bool bindAndListen(u_short);
    bool acceptClient();

    void closeServer();

    void sendData(const char*);
    string receiveData();
};

namespace ConnectT{
    void startClRecv(Connect_Client&);
    void startSvRecv(Connect_Server&);
    void onRecvData(string);

    extern bool recv_flag;
    extern Connect_Client* connect_c;
    extern Connect_Server* connect_s;
};