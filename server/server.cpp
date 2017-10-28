#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <WinSock2.h>
#include <string>
#include <cstring>
#include <list>
#include <fstream>
#include <vector>
using namespace std;

enum commandSet{
    req,
    ack,
    bye,
    no_command
};

void extractData(string fileName);
void initListener(int portNum);
bool waitAndConnect();
bool sendData(string window);
bool feedBack();
int waitAndConnectThread(int portNum);
void encode(int seq , bool isLastSeq , string data);
commandSet selectCommand(char commandPacket[]);
void sepDataAndSend(const string &arg);
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

SOCKET currentConnect , slisten;
void initListener(int portNum){
    addrlen = sizeof(currentAddress);
    currentAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    //Must convert to short because sin_port has been declared as u_short
    currentAddress.sin_port = htons(portNum);
    currentAddress.sin_family = AF_INET;

    slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Socket that listen for new connections
    bind(slisten, (SOCKADDR *) &currentAddress, addrlen);
}
bool waitAndConnect() {

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
    send(currentConnect, window.c_str(), window.length(), NULL);
    return true;
}

const int trueDataSize = expectedPacketLength - 11;
string packet;
int headerSize = 0;
char* flag="7E";
string argument;
list<string> listOfSeperatedData;

commandSet selectCommand(char commandPacket[]){
    int size = strlen(commandPacket);
    char comm[4]={0};
    string commStr;
    char arg[493]={0};
    if(size > 500){
        cout << "<X>:Command too long." << endl;
        return no_command;
    } else{
        for(int i=2 ; i<size ; i++){
            if(i >= 2 and i <= 4){
                comm[i-2] = commandPacket[i];
            } else{
                if(commandPacket[i] == 27){
                    arg[i-5] = commandPacket[i+1];
                    arg[i-4] = commandPacket[i+2];
                    i+=2;
                } else{
                    if(commandPacket[i] == '7' and commandPacket[i+1] == 'E')break;
                    arg[i-5] = commandPacket[i];
                }
            }
        }
        argument = arg;
        commStr = comm;
        memset(commandPacket , 0 , sizeof(commandPacket));
        if(commStr == "REQ") return req;
        if(commStr == "ACK") return ack;
        if(commStr == "BYE") return bye;
        else return no_command;
    }
}

bool feedBack() {
    char ackPacket[20];
    recv(currentConnect, ackPacket, 20 , NULL);
    cout << ackPacket << " ";
    switch(selectCommand(ackPacket)){
        case ack:
            return true;
        default:
            return false;
    }
}

char commandPacket[510]={0};
bool toggleLeave = false;
int commandSize = 0;
int waitAndConnectThread(int portNum){
    initListener(portNum);
    while (true) {
        if (waitAndConnect()) {
            while (true) {
                cout << " -->:Waiting for client command..." << endl;
                commandSize = recv(currentConnect, commandPacket, 510, NULL);
                if(commandSize == -1){
                    cout << "<I>:Now listening on port : " << portNum << endl;
                    listOfSeperatedData.clear();
                    break;
                }
                switch (selectCommand(commandPacket)) {
                    case req:
                        cout << "-<I>:Client request file " << argument << "\n\tNow sending..." << endl;
                        sepDataAndSend(argument);
                        break;
                    case ack:
                        cout << "-<I>:Client ACK receive but... ACK what??" << endl;
                        break;
                    case bye:
                        toggleLeave = true;
                        cout << "<!>:Bye command detected!" << endl;
                        closesocket(currentConnect);
                        break;
                    case no_command:
                        cout << "<X>:Unknown command" << endl;
                        break;
                }
            }
        }
    }
}

void sepDataAndSend(const string &arg){
    extractData(arg);
    int seq = 0 , totalSize = listOfSeperatedData.size();
    for(list<string>::iterator it=listOfSeperatedData.begin(); it != listOfSeperatedData.end() ; it++){
        if(seq == totalSize-1){
            encode(seq , true , *it);
        } else{
            encode(seq , false , *it);
        }
        if (sendData(packet)) {
            if(feedBack()){
                cout << " -<I>:Data receive successfully" << endl;
                seq++;
            }
            else {
                cout << " -<!>:Data receive unsuccessful, resending..." << endl;
                it--;
            }
        }
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
    char eachSepData[trueDataSize+10]={0} , eachChar , similar[10]={0} , ESC = 'a';
    int index=0 , similarity_lvl = 0;
    string eachData;
    while(fileRead.get(eachChar)){
        if(eachChar == flag[similarity_lvl]){
            similar[similarity_lvl] = eachChar;
            similarity_lvl++;
            //When this function sure that detect the flag so replace flag (in this case use "7E")
            //with ESC+flag ("'27'7E")
            if(similarity_lvl == strlen(flag)){
                eachSepData[index] = ESC;
                strcat(eachSepData , similar);
                index+=similarity_lvl+1;
                similarity_lvl=0;
                memset(similar , 0 , sizeof(similar));
            }
        }
        else {
            if(similarity_lvl != 0){
                strcat(eachSepData , similar);
                index+=similarity_lvl;
                similarity_lvl=0;
                memset(similar , 0 , strlen(similar));
            }
            eachSepData[index] = eachChar;
            index++;
            if (index >= trueDataSize - 1) {
                eachData = eachSepData;
                cout << eachData << endl;
                listOfSeperatedData.push_back(eachData);
                memset(eachSepData, 0, sizeof(eachSepData));
                index=0;
            }
        }
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
