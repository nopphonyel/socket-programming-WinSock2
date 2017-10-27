#include <iostream>
#include <winsock2.h>
#include <afxres.h>

using  namespace std;

WSAData wsaData;

bool initWinSock() {
    WORD version = MAKEWORD(2, 1);
    if (WSAStartup(version, &wsaData) != 0) {
        cout << "<X>:Failed to initialize WinSock" << endl;
        return false;
    }
    else if (wsaData.wVersion != version) {
        WSACleanup();
        return false;
    }
    else {
        return true;
    }
}

SOCKET currentConnect;

bool connectToServer(char* ipAddr , int portNum){
    SOCKADDR_IN targetServer;
    int sizeOfSocketAddr = sizeof(targetServer);
    //cout << sizeOfSocketAddr;
    targetServer.sin_family = AF_INET;
    targetServer.sin_addr.s_addr = inet_addr(ipAddr);
    //Must convert to short because sin_port has been declared as u_short
    targetServer.sin_port = htons(portNum);

    SOCKET socket1;
    socket1 = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
    if (connect(socket1 , (SOCKADDR*)&targetServer , sizeOfSocketAddr) == SOCKET_ERROR){
        cout << "<X>:Failed to connect to " << targetServer.sin_addr.S_un.S_addr << endl;
        return false;
    } else{
        cout << "<I>:Successfully connected to " << targetServer.sin_addr.S_un.S_addr << endl;
        currentConnect = socket1;
        return true;
    }
}

bool recvData(char* recvWindow){
    recv(currentConnect , recvWindow , 50 , NULL);
    if(strlen(recvWindow)!=0){
        return true;
    } else{
        return false;
    }
}

int main(){
    char ipAddr[10] = {0};
    char recvText[50]={0};
    int portNum = 0;
    cout << "Please enter IP Address and port number\nIP,PORT>";
    cin >> ipAddr >> portNum;
    if(initWinSock()){
        if(connectToServer(ipAddr , portNum)){
            if(recvData(recvText)) {
                cout << "<I>:Text recieve as : " << recvText
                     << " " << strlen(recvText) << endl;
                send(currentConnect , "ACK" , sizeof("ACK")+1 , NULL);
            } else{
                cout << "<!>:NULL data received" << endl;
            }
        }
    }
    return 0;
}
