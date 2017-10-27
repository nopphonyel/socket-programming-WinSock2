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
bool sendData(char window[150]);
bool feedBack();
void waitAndConnectThread(int portNum);
void encode(int seq , bool isLastSeq , char* data);

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
        cout << "-<I>:Connection from client!" << endl;
        currentConnect = newCon;
        return true;
    }
}

bool sendData(char window[150]) {
    send(currentConnect, window, 150, NULL);
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

int trueDataSize = 150 - 11;
char packet[150]={0};
int headerSize = 0;
char* flag="7E";
string inputData;
typedef vector<char> eachData;
list<eachData> listOfSeperatedData;

void waitAndConnectThread(int portNum){
    if (waitAndConnect(portNum)) {
        cout << "<?>:Please input file name to send\nFILE_NAME>";
        cin >> inputData;
        extractData(inputData);
        int seq = 0 , totalSize = listOfSeperatedData.size();
        for(vector<char> it : listOfSeperatedData ){
            if(seq = totalSize-1){
                encode(seq , true , &it[0]);
            } else{
                encode(seq , false , &it[0]);
            }
            seq++;
        }
        /*if (sendData(packet)) {
            cout << (feedBack() ?
                     " -<I>:Data receive successfully" :
                     " -<!>:Data receive unsuccessful")
                 << endl;
        }*/
    }
}

void encode(int seq , bool isLastSeq , char* data){
    char seqChar[6];
    sprintf(seqChar , "%06d" , seq);
    memset(packet , 0 , strlen(packet));
    strcat(packet , flag);
    if(isLastSeq) strcat(packet , "1");
    else strcat(packet , "0");
    strcat(packet , seqChar);
    strcat(packet , data);
    strcat(packet , flag);
    cout << "---[<SEQ:" << seq << ">:" << packet << "]" <<endl;
}

void extractData(string fileName){
    ifstream fileRead(fileName);
    vector<char> eachSepData;
    while(fileRead.read(&eachSepData[0] , trueDataSize)){
        listOfSeperatedData.push_back(eachSepData);
    }
}

int main() {
    cout << "Please enter port number\nPORT>";
    int portNum = 0;
    cin >> portNum;
    if (initWinSock()) {
        CreateThread(NULL , NULL , (LPTHREAD_START_ROUTINE)waitAndConnect(portNum) , NULL , NULL , NULL);
    }
    system("pause");
    return 0;
}
