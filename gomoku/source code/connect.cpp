#include <winsock2.h>
#include <string>
#include <windows.h>

#include "connect.h"

using std::wstring;
using std::string;

bool ConnectT::recv_flag = false;
Connect_Client* ConnectT::connect_c = nullptr;
Connect_Server* ConnectT::connect_s = nullptr;

Connect_Client::Connect_Client() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    serverAddr.sin_family = AF_INET;
}
Connect_Client::~Connect_Client() { 
    if (connectSocket != INVALID_SOCKET) {
        shutdown(connectSocket, SD_SEND);
        //MessageBox(NULL, L"Client Shutdown", L"debug", MB_OK);
    }
    closesocket(connectSocket);
    connectSocket = INVALID_SOCKET;
    WSACleanup();
}
/*
bool Connect_Client::initializeWSAD() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        return false; // Initialization failed
    }
    return true; // Initialization succeeded
}
bool Connect_Client::initializeSocket() {
    connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connectSocket == INVALID_SOCKET) {
        wstring wstr = L"Failed to create socket\n Error code:";
        wstr += std::to_wstring(WSAGetLastError());
        //MessageBoxW(NULL, wstr.c_str(), L"Debug", MB_OK);
        WSACleanup();
        return false; // Socket creation failed
    }
    return true; // Socket creation succeeded
}
*/
void Connect_Client::initializeServerAddr(const char* ipAddress, int port) {
    serverAddr.sin_addr.s_addr = inet_addr(ipAddress);
    serverAddr.sin_port = htons(port);
}
bool Connect_Client::connectToServer() {
    int result = connect(connectSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR) {
        //MessageBoxW(NULL, L"Failed to connect to server", L"Debug", MB_OK);
        return false;
    }
    //MessageBox(clientConnecthWnd, L"Success connectToServer", L"debug", MB_OK);
    return true;
}
void Connect_Client::sendData(const char* data) {
    send(connectSocket, data, strlen(data)+1, 0);
}
string Connect_Client::receiveData(){
    char buffer[128] = {0};
    string result;
    while (true) {
        int len = recv(connectSocket, buffer, sizeof(buffer), 0);
        if (len <= 0) return "";   // disconnected or error

        for (int i = 0; i < len; i++) {
            if (buffer[i] == '\0') {
                // 收到結尾 → 回傳完整字串
                return result;
            }
            result += buffer[i];
        }
    }
}


Connect_Server::Connect_Server(){
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
}
Connect_Server::~Connect_Server(){
    if (clientSocket != INVALID_SOCKET) {
        shutdown(clientSocket, SD_SEND);
    }
    closesocket(listenSocket);
    listenSocket = INVALID_SOCKET;
    closesocket(clientSocket);
    clientSocket = INVALID_SOCKET;
    WSACleanup();
}
/*
bool Connect_Server::initializeWSAD() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        return false; // Initialization failed
    }
    return true; // Initialization succeeded
}
bool Connect_Server::initializeSocket() {
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        wstring wstr = L"Failed to create socket\n Error code:";
        wstr += std::to_wstring(WSAGetLastError());
        //MessageBoxW(NULL, wstr.c_str(), L"Debug", MB_OK);
        WSACleanup();
        return false; // Socket creation failed
    }
    return true; // Socket creation succeeded
}
*/
bool Connect_Server::bindAndListen(u_short port){

    serverAddr.sin_port = htons(port);
    if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        //MessageBoxW(NULL, L"bind Error", L"debug", MB_OK);
        return false;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        //MessageBoxW(NULL, L"listen Error", L"debug", MB_OK);
        return false;
    }
    return true;
}
bool Connect_Server::acceptClient(){
    int clientSize = sizeof(clientAddr);
    clientSocket = accept(listenSocket, (sockaddr*)&clientAddr, &clientSize);
    if (clientSocket == INVALID_SOCKET) {

        return false;
    }
    //MessageBoxW(serverConnecthWnd, L"Success acceptClient", L"debug", MB_OK);
    return true;
}
void Connect_Server::closeServer(){
    if(listenSocket != INVALID_SOCKET)
        closesocket(listenSocket);
    return;
}
void Connect_Server::sendData(const char* data){
    send(clientSocket, data, strlen(data)+1, 0);
}
string Connect_Server::receiveData(){
    char buffer[128] = {0};
    string result;
    while (true) {
        int len = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (len <= 0) return "";   // disconnected or error

        for (int i = 0; i < len; i++) {
            if (buffer[i] == '\0') {
                // 收到結尾 → 回傳完整字串
                return result;
            }
            result += buffer[i];
        }
    }
}


void ConnectT::startClRecv(Connect_Client& cl){
    ConnectT::recv_flag = true;
    //MessageBoxW(NULL, L"client開始接收信息", L"Error", MB_OK);
    while(ConnectT::recv_flag){
        string s = cl.receiveData();
        if(s.size() > 0)
            ConnectT::onRecvData(s);
        else
            break;
    }
    ConnectT::onRecvData("ServerClosed");
    return;
}

void ConnectT::startSvRecv(Connect_Server& sv){
    ConnectT::recv_flag = true;
    //MessageBoxW(NULL, L"server開始接收信息", L"Error", MB_OK);
    while(ConnectT::recv_flag){
        string s = sv.receiveData();
        if(s.size() > 0)
            ConnectT::onRecvData(s);
        else
            break;
    }
    ConnectT::onRecvData("ClientClosed");
    return;
}

void ConnectT::onRecvData(string s){
    char* buf = new char[s.size() + 1];
    strcpy_s(buf, s.size() + 1, s.c_str());

    HWND hWnd;
    if(clientConnecthWnd) hWnd = clientConnecthWnd;
    if(serverConnecthWnd) hWnd = serverConnecthWnd;

    PostMessageA(hWnd, WM_APP_TCP_MSG, 0, (LPARAM)buf);
    return;
}