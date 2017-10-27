#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <WinSock2.h>
#include <string>
#include <cstring>
#include <list>
#include <fstream>
#include <vector>

using namespace std;
void extractData(string fileName);
bool waitAndConnect(int portNum);
bool sendData(string window);
bool feedBack();
int waitAndConnectThread(int portNum);
void encode(int seq , bool isLastSeq , string data);
void stringCut(int from , int to , char srcString[] ,  char *returnString);

WSAData wsaData;
SOCKADDR_IN currentAddress;
int addrlen = 0;
const int expectedPacketLength = 150;

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
        cout << "-<I>:Connection from client!" << endl;
        currentConnect = newCon;
        return true;
    }
}

bool sendData(string window) {
    send(currentConnect, window.c_str(), expectedPacketLength, NULL);
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

const int trueDataSize = expectedPacketLength - 11;
string packet;
int headerSize = 0;
char* flag="7E";
string inputData;
list<string> listOfSeperatedData;

int waitAndConnectThread(int portNum){
    if (waitAndConnect(portNum)) {
        cout << "<?>:Please input file name to send\nFILE_NAME>";
        cin >> inputData;
        extractData(inputData);
        int seq = 0 , totalSize = listOfSeperatedData.size();
        for(list<string>::iterator it=listOfSeperatedData.begin(); it != listOfSeperatedData.end() ; it++){
            if(seq == totalSize-1){
                encode(seq , true , *it);
            } else{
                encode(seq , false , *it);
            }
            if (sendData(packet)) {
                cout << (feedBack() ?
                         " -<I>:Data receive successfully" :
                         " -<!>:Data receive unsuccessful")
                     << endl;
            }
            seq++;
        }
        closesocket(currentConnect);
    }
}

void encode(int seq , bool isLastSeq , string data){
    char seqChar[6];
    sprintf(seqChar , "%06d" , seq);
    string seqStr = seqChar;
    packet = "";
    packet = packet+flag;
    if(isLastSeq) packet+="1";
    else packet+="0";
    packet+=seqStr;
    packet+=data.c_str();
    packet+=flag;
    cout << seqChar << " : " << data << endl;
    //cout << "---[<SEQ:" << seq << ">:" << packet << "] : len = " << data.length() <<endl;
}

void extractData(string fileName){
    ifstream fileRead(fileName);
    char eachSepData[trueDataSize+1]={0};
    string eachData;
    while(fileRead.read(eachSepData , trueDataSize)){
        eachData = eachSepData;
        cout << eachData;
        listOfSeperatedData.push_back(eachData);
        memset(eachSepData , 0 , trueDataSize);
    }
    eachData = eachSepData;
    cout << eachData << endl;
    listOfSeperatedData.push_back(eachData);
}

int main() {
    cout << "Please enter port number\nPORT>";
    int portNum = 0;
    cin >> portNum;
    if (initWinSock()) {
        //CreateThread(NULL , NULL , (LPTHREAD_START_ROUTINE)waitAndConnectThread(portNum) , NULL , NULL , NULL);
        waitAndConnectThread(portNum);
    }
    system("pause");
    return 0;
}

void stringCut(int from , int to , char srcString[] , char *returnString){
    int max = strlen(srcString);
    if(to > max) to = max;
    for(int i=from ; i <= to ; i++){
        returnString[i] = srcString[i];
    }
}
