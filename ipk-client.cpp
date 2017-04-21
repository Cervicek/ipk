#include <iostream>
#include <string>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
//#include <time.h>
#include <fstream>
#include <cstring>
#include <limits>
#include <sstream>
#include <arpa/inet.h>
#include <openssl/md5.h>
#include <math.h>
#include <stdexcept>

#define BUFSIZE 1024

using namespace std;

string sayHello(string *message, string loginS){
    //998a981765c582cbd5754ae25b4f43d3
    //std::cout << loginS << '\n';
     string msg(
            "HELLO " + loginS + "\n"
        );
     *message = msg;
     //std::cout << *message << '\n';
     return *message;
}

string sayResult(string resultSTR, string *message){
    string msg(
            "RESULT " + resultSTR + "\n"
        );
    *message = msg;
    return *message;
}

string sayErrResult(string *message){
    string msg(
            "RESULT ERROR\n"
        );
    *message = msg;
    return *message;
}

string typeDecide(string text, string type){
    int found = text.find(" ");
    if(found > -1){
        return type = text.substr(0, found);
    }
    return "error";
}

string getSecret(string text){
    int found = text.find(" ");
    if(found > -1){
        return text = text.substr(found+1);
    }
    return "error";
}

int getEverythingForTask(string text, double *number1, string *op, double *number2){
    string tmp;
    string iMIN;
    string iMAX;
    int imin = std::numeric_limits<int>::min(); // minimum value
    int imax = std::numeric_limits<int>::max();
    int tmpINT1;
    int tmpINT2;
    size_t stringLength1;
    size_t stringLength2;
    double intpart1, intpart2, fractpart1, fractpart2;
    string check1, check2;

    //std::cout << "1 " + text << '\n';
    int foundEDNL = text.find("\n");
    if(foundEDNL > -1){
      text = text.substr(0, foundEDNL);
    }

    //std::cout <<  text << '\n';
    int foundCut = text.find(" ");
    if(foundCut > -1){
        tmp = text.substr(foundCut+1);
    }

    int found1 = tmp.find(" ");
    int found2 = tmp.find_last_of(" ");
    if(found1 > -1 && found2 > -1){

      try{
        *number1 = stod(tmp.substr(0, found1));
      }catch(const std::invalid_argument& ia){
        return -2;
      }
        fractpart1 = modf (*number1 , &intpart1);
        if(fractpart1 != 0.0){
          return -1;
        }
        *op = tmp.substr(found1+1, found2-found1-1);

        try{
        *number2 = stod(tmp.substr(found2+1));
      }catch(const std::invalid_argument& ia){
        return -2;
      }
        fractpart2 = modf (*number2 , &intpart2);
        if(fractpart2 != 0.0){
          return -1;
        }
        return 0;
    }else{
        return -1;
    }
}

string calculate(double number1, string op, double number2, double *result){
    /*if(number1 == INT_MAX || number1 == INT_MIN || number2 == INT_MAX || number2 == INT_MIN){
        return "RESULT ERROR";
    }*/
    /*double n1 = double(number1);                pouze pokud verze s intem (upravit number na n nize)
    double n2 = double(number2);*/

    if(op == "+"){
        *result = number1 + number2;
        return "ok";
    }else if(op == "-"){
        *result = number1 - number2;
        return "ok";
    }else if(op == "*"){
        *result = number1 * number2;
        return "ok";
    }else if(op == "/"){
        //cout << number1 << endl;
        if(number2 == 0){
            return "RESULT ERROR";
        }
        *result = number1 / number2;
        return "ok";
    }else{
        return "RESULT ERROR";
    }
}

string cut(string resultSTR){
    string tmp;
    int foundCut = resultSTR.find(".");
    if (foundCut > -1){
        tmp = resultSTR.substr(0, foundCut+3);
        resultSTR = tmp;
        return resultSTR;
    }
    else{
        return "ERROR";
    }
}

int main(int argc, char *argv[])
{
    string text;      // Odesílaný a přijímaný text
    hostent *host;              // Vzdálený počítač;
    hostent *server;
    sockaddr_in serverSock;     // Vzdálený "konec potrubí"
    int mySocket;               // Soket
    const char *port = "55555";                // Číslo portu
    char buf[BUFSIZE];          // Přijímací buffer
    int size;                   // Počet přijatých a odeslaných bytů
    int sockfd;
    string address;
    string message;
    string decide;
    string type;
    double number1;
    string op;
    double number2;
    string secret;
    int task;
    double result;
    string resultSTR;
    string resultErr;
    struct in6_addr addr6;
    struct addrinfo hints, *res = NULL;
    struct in6_addr serveraddr;
    int r = 0;

    memset(&hints, 0, sizeof hints);
    hints.ai_flags = AI_NUMERICSERV;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    unsigned char digest[16];
    const char* login = "xcervi22";
    string loginS;

    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, login, strlen(login));
    MD5_Final(digest, &ctx);

    char mdString[33];
    for (int i = 0; i < 16; i++)
        sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);


    loginS = string(mdString);

   if (argc != 2)
    {
        cerr << "Syntaxe:\n\t" << argv[0]
                  << " " << "adresa port" << endl;
        return -1;
    }else{
        address = argv[1];
    }

    // Vytvoříme soket
    /*if ((mySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        cerr << "Nelze vytvořit soket" << endl;
        return -1;
    }

    serverSock.sin_family = AF_INET;
    serverSock.sin_port = htons(port);
    memcpy(&(serverSock.sin_addr), host->h_addr, host->h_length);*/

    if((r = inet_pton(AF_INET, address.c_str(), &serveraddr))==1){
        hints.ai_family = AF_INET;
        hints.ai_flags |= AI_NUMERICHOST;
    }else{
        r = inet_pton(AF_INET6, address.c_str(), &serveraddr);
        if(r==1){
            hints.ai_family = AF_INET6;
            hints.ai_flags |= AI_NUMERICHOST;
        }
    }

    if((r = getaddrinfo(address.c_str(), port, &hints, &res)) != 0){
      const char *err = gai_strerror(r);
      cout << err << endl;
        printf("Host not found");
        if(r == EAI_SYSTEM){
            perror("getaddrinfo() failed");
        }
    }

    mySocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(mySocket<0){
        perror("socket failed");
    }

    // Připojení soketu
    if (connect(mySocket, res->ai_addr, res->ai_addrlen) == -1)
    {
        cerr << "Nelze navázat spojení" << endl;
        return -1;
    }

    message = sayHello(&text, loginS);
    //cout << "-> " + message;
    if ((size = send(mySocket, message.c_str(), message.size(), 0)) == -1)
    {
        cerr << "Unknown error.\n" << endl;
        return -1;
    }

    do{
        text = "";
        while ((size = read(mySocket, buf, BUFSIZE)) > 0)
        {
            text.append(buf, size);
            break;
        }

        decide = typeDecide(text, type);

        if(decide == "SOLVE"){
            //cout << "<- " + text;
            task = getEverythingForTask(text, &number1, &op, &number2);
            if(task == 0){
                resultErr = calculate(number1, op, number2, &result);
                if(resultErr == "ok"){
                    resultSTR = to_string(result);
                    resultSTR = cut(resultSTR);
                    message = sayResult(resultSTR, &message);
                    cout << "-> " + message;
                    if ((size = send(mySocket, message.c_str(), message.size(), 0)) == -1)
                    {
                        cerr << "Unknown error.\n" << endl;
                        return -1;
                    }
                }else{
                    message = sayErrResult(&message);
                    cout << "-> " + message;
                    if ((size = send(mySocket, message.c_str(), message.size(), 0)) == -1)
                    {
                        cerr << "Unknown error.\n" << endl;
                        return -1;
                    }
                }
            }else if(task == -1){
                message = sayErrResult(&message);
                cout << "-> " + message;
                if ((size = send(mySocket, message.c_str(), message.size(), 0)) == -1)
                {
                    cerr << "Unknown error.\n" << endl;
                    return -1;
                }
            }
        }else if(decide == "BYE"){
            secret = getSecret(text);
                cout << secret;
        }
    }while(decide != "BYE");

    close(mySocket);
    return 0;
}
