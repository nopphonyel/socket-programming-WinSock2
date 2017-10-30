#include <iostream>
#include <WinSock2.h>
#include <list>
#include <fstream>
#include <thread>
#include <mutex>
#include <condition_variable>

#define TIMEOUT_FILE_SEND_SEC 0.5
#define TIMEOUT_INACTIVITY 30
using namespace std;

enum commandSet {
    req,
    ack,
    bye,
    no_command
};

enum sendMode {
    stop_and_wait,
    selective_repeat
};

void extDataStopNWait(const string &fileName);

void initListener(int portNum);

bool waitAndConnect();

bool sendData(const string &window);

bool feedBack();

int waitAndConnectThread(int portNum);

void encode(int seq, bool isLastSeq, const string &data);

commandSet selectCommand(char commandPacket[]);

void sendInBackground(string fileName);

void extDataSelectiveRepeat(string fileName);

void sepDataAndSend(const string &arg);

void removePacketSeq(int seq);

void resendThread();

WSAData wsaData;
SOCKADDR_IN currentAddress;
int addrlen = 0;
const int expectedPacketLength = 150, MAX_ATTEMP = 4, SND_BUFF_SIZE = 10;
sendMode currentMode;
char flag[2] = "~";

class PacketPack {
public:
    int attemp;
    int ttr;
    int seq;
    string packet;

    PacketPack() {
        attemp = 0;
        ttr = 0;
        seq = 0;
        packet = "";
    }

    void increaseTTR() {
        ttr++;
    }

    void increaseATTMP() {
        attemp++;
    }
};

bool initWinSock() {
    WORD version = MAKEWORD(2, 1);
    if (WSAStartup(version, &wsaData) != 0) {
        cout << "<X>:Failed to initialize WinSock" << endl;
        return false;
    } else return wsaData.wVersion == version;
}

SOCKET currentConnect, slisten;

void initListener(int portNum) {
    addrlen = sizeof(currentAddress);
    currentAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    //Must convert to short because sin_port has been declared as u_short
    currentAddress.sin_port = htons((u_short) portNum);
    currentAddress.sin_family = AF_INET;

    slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Socket that listen for new connections
    bind(slisten, (SOCKADDR *) &currentAddress, addrlen);
}

bool waitAndConnect() {
    cout << "<I>:Waiting for incoming connection" << endl;
    listen(slisten, SOMAXCONN);

    SOCKET newCon;
    DWORD timeOut = TIMEOUT_INACTIVITY * 1000;
    newCon = accept(slisten, (SOCKADDR *) &currentAddress, &addrlen);
    if (newCon == 0) {
        cout << "<X>:Failed to accept client's connection." << endl;
        return false;
    } else {
        cout << "-<I>:Connection from client!" << endl;
        currentConnect = newCon;
        setsockopt(currentConnect, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeOut, sizeof(timeOut));
        return true;
    }
}

bool sendData(const string &window) {
    auto *debug_packet = const_cast<char *>(window.c_str());
    send(currentConnect, window.c_str(), (int) window.length(), NULL);
    return true;
}

const int trueDataSize = expectedPacketLength - 11;
string packet;
string argument;
list<PacketPack> listOfSeperatedData;

commandSet selectCommand(char commandPacket[]) {
    auto size = (int) strlen(commandPacket), flag_size = (int) strlen(flag);
    char comm[4] = {0};
    string commStr;
    char arg[493] = {0};
    if (size > 500) {
        cout << "<X>:Command too long." << endl;
        return no_command;
    } else {
        for (int i = flag_size; i < size; i++) {
            if (i >= flag_size and i <= flag_size + 2) {
                comm[i - flag_size] = commandPacket[i];
            } else {
                //Found the end of commandPacket
                if (commandPacket[i] == flag[0] && commandPacket[i + 1] == '\0') {
                    break;
                } else {
                    //Skip first flag and obtain another one as argument
                    if (commandPacket[i] == flag[0]) {
                        arg[i - (flag_size + 3)] = commandPacket[i + 1];
                        i++;
                    }
                    arg[i - (flag_size + 3)] = commandPacket[i];
                }
            }
        }
        argument = arg;
        commStr = comm;
        memset(commandPacket, 0, sizeof(commandPacket));
        if (commStr == "REQ") return req;
        if (commStr == "ACK") return ack;
        if (commStr == "BYE") return bye;
        else return no_command;
    }
}

bool feedBack() {
    char ackPacket[20];
    recv(currentConnect, ackPacket, 20, NULL);
    cout << "[Client-COMMAND]:" << ackPacket << ",";
    switch (selectCommand(ackPacket)) {
        case ack:
            cout << " -<I>:Packet receive" << endl;
            return true;
        default:
            cout << " -<!>:Packet unreceived or corrupted, Resending..." << endl;
            return false;
    }
}

char commandPacket[510] = {0};
int commandSize = 0;

int waitAndConnectThread(int portNum) {
    DWORD timeOut = (DWORD) TIMEOUT_FILE_SEND_SEC * 1000;
    initListener(portNum);
    while (true) {
        if (waitAndConnect()) {
            while (true) {
                cout << " -->:Waiting for client command..." << endl;
                commandSize = recv(currentConnect, commandPacket, 510, NULL);
                if (commandSize == -1) {
                    cout << "<I>:Now listening on port : " << portNum << endl;
                    listOfSeperatedData.clear();
                    break;
                }
                switch (selectCommand(commandPacket)) {
                    case req:
                        cout << "-<I>:Client request file " << argument << "\n\tNow sending..." << endl;
                        setsockopt(currentConnect, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeOut, sizeof(timeOut));
                        if (currentMode == stop_and_wait) extDataStopNWait(argument);
                        if (currentMode == selective_repeat) extDataSelectiveRepeat(argument);
                        //sepDataAndSend(argument);
                        break;
                    case ack:
                        cout << "-<I>:Client ACK receive : ACK on packet : " << argument << ":"
                             << atoi(argument.c_str()) << endl;
                        break;
                    case bye:
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

void sepDataAndSend(const string &arg) {
    //extractData(arg);
    int seq = 0, totalSize = (int) listOfSeperatedData.size();
    for (auto it = listOfSeperatedData.begin(); it != listOfSeperatedData.end(); it++) {
        if (seq == totalSize - 1) {
            //encode(seq , true , *it);
        } else {
            //encode(seq , false , *it);
        }
        if (sendData(packet)) {
            if (feedBack()) {
                cout << " -<I>:Data receive successfully" << endl;
                seq++;
            } else {
                cout << " -<!>:Data receive unsuccessful, resending..." << endl;
                it--;
            }
        }
    }
}

void encode(int seq, bool isLastSeq, const string &data) {
    char seqChar[6];
    sprintf(seqChar, "%06d", seq);
    string seqStr = seqChar;
    packet.clear();
    packet = packet + flag;
    if (isLastSeq) packet += "1";
    else packet += "0";
    packet += seqStr;
    packet += data;
    packet += flag;
    if (!isLastSeq)cout << "<I>:Encoded SEQ:" << seqChar << " ";
    else cout << "<I>:Encoded last SEQ:" << seqChar << " ";
    //cout << "---[<SEQ:" << seq << ">:" << packet << "] : len = " << data.length() <<endl;
}

int lastSequence = -1;
bool isSending = false;
mutex mu;
condition_variable cond;

void sendInBackground(string fileName) {
    ifstream fileRead(fileName);
    char eachSepData[trueDataSize + 10] = {0}, eachChar, similar[10] = {0};
    int index = 0, similarity_lvl = 0, seq = 0, attemp = 0;
    bool feedBackResult = true;
    PacketPack eachPack = PacketPack();
    string eachData;
    while (fileRead.get(eachChar)) {
        if (eachChar == flag[similarity_lvl]) {
            similar[similarity_lvl] = eachChar;
            similarity_lvl++;
            //When this function sure that detect the flag so replace flag (in this case use "~")
            //with flag+flag ('~''~')
            if (similarity_lvl == strlen(flag)) {
                strcat(eachSepData, flag);
                strcat(eachSepData, similar);
                index += 2 * similarity_lvl;
                similarity_lvl = 0;
                memset(similar, 0, sizeof(similar));
            }
        } else {
            if (similarity_lvl != 0) {
                strcat(eachSepData, similar);
                index += similarity_lvl;
                similarity_lvl = 0;
                memset(similar, 0, strlen(similar));
            }
            eachSepData[index] = eachChar;
            index++;
            //When length is enough to be send
            if (index >= trueDataSize - 1) {

                eachData = eachSepData;
                encode(seq, false, eachData);
                memset(eachSepData, 0, sizeof(eachSepData));
                index = 0;
                unique_lock<mutex> locker(mu);
                cout << "--RESOURCE LOCKED! --:";
                if (listOfSeperatedData.size() >= SND_BUFF_SIZE) {
                    //Wait for clearing
                    cout << "<!>Buffer full waiting for clearing..." << endl;
                    cond.wait(locker, []() { return listOfSeperatedData.size() < SND_BUFF_SIZE; });
                }
                eachPack.seq = seq;
                eachPack.packet = packet;
                cout << "[SEQ:" << seq << "]";
                listOfSeperatedData.push_back(eachPack);
                cout << "--RESOURCE UNLOCKED! --:" << endl;
                locker.unlock();
                sendData(packet);
                seq++;
            }
        }
    }
    eachData = eachSepData;
    encode(seq, true, eachData);
    lastSequence = seq;
    unique_lock<mutex> locker(mu);
    cout << "--RESOURCE LOCKED! --:";
    if (listOfSeperatedData.size() >= SND_BUFF_SIZE) {
        //Wait for clearing
        cout << "<!>Buffer full waiting for clearing..." << endl;
        cond.wait(locker, []() { return listOfSeperatedData.size() < SND_BUFF_SIZE; });
        //lock
    }
    eachPack.seq = seq;
    eachPack.packet = packet;
    listOfSeperatedData.push_back(eachPack);
    sendData(packet);

    cout << "--RESOURCE UNLOCKED! --:" << endl;
    locker.unlock();
    isSending = false;
}

void extDataSelectiveRepeat(string fileName) {
    //This constructor will start thread
    isSending = true;
    thread readAndSend(sendInBackground, fileName);
    thread resenderCheck(resendThread);
    int ackSeq = 0, recvResult;
    char ackPacket[20] = {0};
    do {
        recvResult = recv(currentConnect, ackPacket, sizeof(ackPacket), NULL);
        if (recvResult == -1) {
            cout << "  -> SOCKET Closed!" << endl;
            break;
        }
        if (selectCommand(ackPacket) == ack) {
            ackSeq = atoi(argument.c_str());
            cout << "<I>:" << ackSeq << endl;
            //Remove item
            removePacketSeq(ackSeq);
        }
    } while (ackSeq != lastSequence || !listOfSeperatedData.empty());
    cout << " --File successfully sent!" << endl;
    if(readAndSend.joinable()) readAndSend.join();
    if(resenderCheck.joinable()) resenderCheck.join();
}

void removePacketSeq(int seq) {
    unique_lock<mutex> locker(mu);
    for (auto it = listOfSeperatedData.begin(); it != listOfSeperatedData.end(); it++) {
        PacketPack eachPack = *it;
        if (eachPack.seq == seq) {
            cout << "   Remove : " << seq << endl;
            it = listOfSeperatedData.erase(it);
        }
    }
    locker.unlock();
    cond.notify_all();
}

void resendThread() {
    DWORD wait = 1000;
    int ttr = 0;
    while (isSending || !listOfSeperatedData.empty()) {
        Sleep(wait);
        PacketPack eachPack;
        unique_lock<mutex> locker(mu);
        for (auto it = listOfSeperatedData.begin(); it != listOfSeperatedData.end(); it++) {
            eachPack = *it;
            sendData(eachPack.packet);
        }
        locker.unlock();
    }
    //Reset last sequence
    lastSequence = -1;
}

void extDataStopNWait(const string &fileName) {
    ifstream fileRead(fileName);
    char eachSepData[trueDataSize + 10] = {0}, eachChar, similar[10] = {0};
    int index = 0, similarity_lvl = 0, seq = 0, attemp = 0;
    bool feedBackResult = true;
    string eachData;
    while (fileRead.get(eachChar)) {
        if (eachChar == flag[similarity_lvl]) {
            similar[similarity_lvl] = eachChar;
            similarity_lvl++;
            //When this function sure that detect the flag so replace flag (in this case use "~")
            //with flag+flag ('~''~')
            if (similarity_lvl == strlen(flag)) {
                strcat(eachSepData, flag);
                strcat(eachSepData, similar);
                index += 2 * similarity_lvl;
                similarity_lvl = 0;
                memset(similar, 0, sizeof(similar));
            }
        } else {
            if (similarity_lvl != 0) {
                strcat(eachSepData, similar);
                index += similarity_lvl;
                similarity_lvl = 0;
                memset(similar, 0, strlen(similar));
            }
            eachSepData[index] = eachChar;
            index++;
            //When length is enough to be send
            if (index >= trueDataSize - 1) {
                eachData = eachSepData;
                encode(seq, false, eachData);
                seq++;
                //listOfSeperatedData.push_back(eachData);
                memset(eachSepData, 0, sizeof(eachSepData));
                index = 0;
                do {
                    if (!feedBackResult) {
                        Sleep(1000);
                        attemp++;
                        if (attemp >= MAX_ATTEMP + 1) break;
                        else cout << "<ATTEMP:" << attemp << ">";
                    } else attemp = 0;
                    sendData(packet);
                    feedBackResult = feedBack();
                } while (!feedBackResult);
                if (attemp >= MAX_ATTEMP + 1) break;
            }
        }
    }
    eachData = eachSepData;
    if (attemp < MAX_ATTEMP) {
        encode(seq, true, eachData);
        do {
            if (!feedBackResult) {
                Sleep(100);
                attemp++;
                if (attemp >= MAX_ATTEMP + 1) break;
                else cout << "<ATTEMP:" << attemp << ">";
            } else attemp = 0;
            sendData(packet);
            feedBackResult = feedBack();
        } while (!feedBackResult);
    }
    //listOfSeperatedData.push_back(eachData);
}

void testMethod(){
    PacketPack packer = PacketPack();
    packer.seq=3;
    cout << packer.seq;
    listOfSeperatedData.push_back(packer);
    list<PacketPack>::iterator it=listOfSeperatedData.begin();
    packer = *it;
    packer.seq++;
    packer = *it;
    cout << packer.seq;
}

int main() {
    int mode = -1;
    testMethod();
    while (mode != 0 && mode != 1) {
        cout << "Please select mode\n\t\'0\' : Stop and wait mode\n\t\'1\' : Selective Repeat mode\nMODE>";
        cin >> mode;
        if (mode == 0) currentMode = stop_and_wait;
        else if (mode == 1) currentMode = selective_repeat;
        else cout << "<X>:Incorrect choice\n";
    }
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
