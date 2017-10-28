#include <iostream>
#include <winsock2.h>
#include <afxres.h>

using namespace std;

WSAData wsaData;

bool initWinSock() {
    WORD version = MAKEWORD(2, 1);
    if (WSAStartup(version, &wsaData) != 0) {
        cout << "<X>:Failed to initialize WinSock" << endl;
        return false;
    } else if (wsaData.wVersion != version) {
        WSACleanup();
        return false;
    } else {
        return true;
    }
}

SOCKET currentConnect;

bool connectToServer(char *ipAddr, int portNum) {
    SOCKADDR_IN targetServer;
    int sizeOfSocketAddr = sizeof(targetServer);
    //cout << sizeOfSocketAddr;
    targetServer.sin_family = AF_INET;
    targetServer.sin_addr.s_addr = inet_addr(ipAddr);
    //Must convert to short because sin_port has been declared as u_short
    targetServer.sin_port = htons(portNum);

    SOCKET socket1;
    socket1 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connect(socket1, (SOCKADDR *) &targetServer, sizeOfSocketAddr) == SOCKET_ERROR) {
        cout << "<X>:Failed to connect to " << targetServer.sin_addr.S_un.S_addr << endl;
        return false;
    } else {
        cout << "<I>:Successfully connected to " << targetServer.sin_addr.S_un.S_addr << endl;
        currentConnect = socket1;
        return true;
    }
}

bool recvData(char *recvWindow) {
    recv(currentConnect, recvWindow, 155, NULL);
    if (strlen(recvWindow) != 0) {
        return true;
    } else {
        return false;
    }
}

char recvPacket[155] = {0};
int waitForReceive(char *ipAddr , int portNum) {
    string reqCommand;
    if (initWinSock()) {
        if (connectToServer(ipAddr, portNum)) {
            cout << "<I>:Connected to server!, please specify file name\nFILE_NAME>";
            cin >> reqCommand;
            reqCommand = "7EREQ" + reqCommand + "7E";
            send(currentConnect , reqCommand.c_str() , reqCommand.length() , NULL);
            while(true) {
                if (recvData(recvPacket)) {
                    cout << "<I>:" << recvPacket << endl;
                    send(currentConnect, "7EACK-7E", sizeof("7EACK-7E") + 1, NULL);
                    if(recvPacket[2] == '1'){
                        send(currentConnect, "7EBYE7E", sizeof("7EBYE7E") + 1, NULL);
                        break;
                    }
                }
            }
        }
    }
};

int main() {
    char ipAddr[10] = {0};
    int portNum = 0;
    cout << "Please enter IP Address and port number\nIP,PORT>";
    cin >> ipAddr >> portNum;
    waitForReceive(ipAddr , portNum);
    system("pause");
    return 0;
}
