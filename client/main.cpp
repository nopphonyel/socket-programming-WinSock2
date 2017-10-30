#include <iostream>
#include <winsock2.h>
#include <string>
#include <afxres.h>
#include <fstream>

using namespace std;

WSAData wsaData;
const int expectedPacketLength = 150;
const int MAX_PACKET_LENGTH = expectedPacketLength + 10;
char flag[2] ="~";
string argument;


int getSequenceNum(char commandPacket[]) {
    int seq=0;
    char num[7]={0};
    for(int i=2 ; i<8 ; i++){
        num[i-2] = commandPacket[i];
    }
    seq=atoi(num);
    return seq;
}

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
    socket1 = socket(AF_INET, SOCK_STREAM, IPPROTO_UDP);
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
    recv(currentConnect, recvWindow, MAX_PACKET_LENGTH, NULL);
    if (strlen(recvWindow) != 0) {
        return true;
    } else {
        return false;
    }
}

string getData(char packet[]){
    int len = (int)strlen(packet);
    char dataCStr[expectedPacketLength] = {0};
    string dataStr;
    for(int i=8 ; i<len-1 ; i++){
        dataCStr[i-8] = packet[i];
    }
    dataStr = dataCStr;
    return dataStr;
}

char recvPacket[expectedPacketLength] = {0};
int waitForReceive(char *ipAddr , int portNum) {
    char reqCommand[500];
    char ackCommand[12]={0} , reqComm[501];
    string data;
    int seq=0;
    if (initWinSock()) {
        if (connectToServer(ipAddr, portNum)) {
            cout << "<I>:Connected to server!, please specify file name\nFILE_NAME>";
            cin >> reqCommand;
            sprintf(reqComm , "~REQ%s~" , reqCommand);
            send(currentConnect , reqComm  , sizeof(reqComm) , NULL);
            ofstream outFile(reqCommand);
            while(true) {
                //Sleep(100);
                if (recvData(recvPacket)) {
                    cout << "<I>:" << recvPacket << endl;
                    seq = getSequenceNum(recvPacket);
                    memset(ackCommand , 0 , strlen(ackCommand));
                    sprintf(ackCommand , "~ACK%d~" , seq);
                    cout << ackCommand << endl;
                    data = getData(recvPacket);
                    outFile << data;
                    send(currentConnect, ackCommand, 12, NULL);
                    if(recvPacket[1] == '1'){
                        send(currentConnect, "~BYE~", sizeof("~BYE~"), NULL);
                        break;
                    }
                    memset(recvPacket , 0 , sizeof(recvPacket));
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
