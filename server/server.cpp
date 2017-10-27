#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <WinSock2.h>
#include <string>
#include <cstring>

using namespace std;

WSAData wsaData;
SOCKADDR_IN currentAddress;
int addrlen = 0;

bool initWinSock() {
    WORD version = MAKEWORD(2, 1);
    if (WSAStartup(version, &wsaData) != 0) {
        cout << "<X>:Failed to initialize WinSock" << endl;
        return false;
    } else if (wsaData.wVersion != version) {
        return false;
    } else {
        return true;
    }
}

SOCKET currentConnect;

bool waitAndConnect(int portNum) {

    addrlen = sizeof(currentAddress);
    currentAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    //Must convert to short because sin_port has been declared as u_short
    currentAddress.sin_port = htons(portNum);
    currentAddress.sin_family = AF_INET;

    SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Socket that listen for new connections
    bind(slisten, (SOCKADDR *) &currentAddress, addrlen);
    cout << "<I>:Waiting for incoming connection" << endl;
    listen(slisten, SOMAXCONN);

    SOCKET newCon;
    newCon = accept(slisten, (SOCKADDR *) &currentAddress, &addrlen);
    if (newCon == 0) {
        cout << "<X>:Failed to accept client's connection." << endl;
        return false;
    } else {
        cout << "<I>:Client connected! : " << currentAddress.sin_addr.s_addr << "\nnewCon int : " << newCon << endl;
        currentConnect = newCon;
        return true;
    }
}

bool sendData(char window[50]) {
    send(currentConnect, window, 50, NULL);
    cout << " -> <DEBUG> : " << strlen(window) << endl;
    return true;
}

bool feedBack() {
    char ack[5];
    recv(currentConnect, ack, sizeof(ack), NULL);
    if (strcmp("ACK", ack) == 0) {
        return true;
    } else {
        return false;
    }
}

int main() {
    cout << "Please enter port number\nPORT>";
    int portNum = 0;
    cin >> portNum;
    if (initWinSock()) {
        if (waitAndConnect(portNum)) {
            if (sendData("sizeofDataWillBeEncodedHere")) {
                cout << (feedBack() ?
                         " -<I>:Data receive successfully" :
                         " -<!>:Data receive unsuccessful")
                     << endl;
            }
        }
    }
    system("pause");
    return 0;
}