// ICMP.cpp : Defines the entry point for the application.
//


#include "ICMP.h"

using namespace std;

int main()
{

#define DEBUG

//#define PING
//#define PINGS
#define TRACE


#ifdef PING

    ping("8.8.8.8");

#endif


#ifdef PINGS
    unsigned int ttl_val = 56;

    SOCKET sock;
    WSADATA wsa;

    WSAStartup(MAKEWORD(2, 2), &wsa);
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    setsockopt(sock, IPPROTO_IP, IP_TTL,
        (char*)&ttl_val, sizeof(ttl_val));


//        ping(sock, "8.8.8.8", 4);
       // ping("8.8.8.8", 4);
    int id = 0;

    std::thread th1([&sock, &id]() {
        //ping(sock, "8.8.8.8", id, 40);
        });

    std::thread th2([&sock, &id]() {
        //ping(sock, "8.8.4.4", id, 40);
        });

    std::thread th3([&sock, &id]() {
        ping(sock, "8.8.4.4", id, 40);
        });

    std::thread th4([&sock, &id]() {
        ping(sock, "139.130.4.5", id, 40);
        });

    th1.join();
    th2.join();
    th3.join();
    th4.join();


    //ping("8.8.8.8");

#endif


#ifdef TRACE

    //auto vec = traceroute("8.8.8.8");
    auto vec = traceroute("82.209.240.241");

    for (auto it : vec)
    {
        std::cout << inet_ntoa(it) << "\n";
    }

#endif


    return 0;
}
