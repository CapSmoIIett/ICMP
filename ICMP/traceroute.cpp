
#include "ICMP.h"


std::vector<in_addr> traceroute(char* ip)
{
    int i = 0;
    int bytes = 0;
    int received = 0;
    int nSequence = 0;
    unsigned int ttl_val = 56;

    std::vector<in_addr> result;

    SOCKET sock;
    WSADATA wsa;
    struct sockaddr_in dest;
    struct sockaddr_in rtrn;

    packet rec;
    ping_pkt pkt;

    struct recv_packet
    {
        //ip
    };

    fd_set fds;

    
    time_t t_min = 0xFFF;
    time_t t_max = 0;
    time_t t_av = 0;

    WSAStartup(MAKEWORD(2, 2), &wsa);
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    int on = 1;



    int  rc;

    SOCKADDR_IN local;

    local.sin_addr.S_un.S_addr = INADDR_ANY;

    local.sin_family = AF_INET;
    local.sin_port = htons(80);


    //auto q = bind(sock, (struct sockaddr*)&local, sizeof(local));
    //auto q = setsockopt(sock, IPPROTO_IP, IP_HDRINCL, (const char*)&on, sizeof(on));

    std::cout << "Pinging " << ip << " with " << sizeof(ping_pkt) << " bytes of data:" << "\n\n";

    for (unsigned int hop = 5; hop < 256; hop++)
    {

        //Sequence number is incremented with each request, this should be different
        //so we can match a response with a request
        pkt.hdr.byCode = 0;       //Zero for ICMP echo and reply messages
        pkt.hdr.nSequence = htons(nSequence++);
        pkt.hdr.byType = ICMP_ECHO;       //Eight for ICMP echo message
        pkt.hdr.nChecksum = 0;    //Checksum is calculated later ono 
        pkt.hdr.nId = GetCurrentProcessId();


        for (i = 0; i < PING_PKT_S - sizeof(ICMPheader) - 1; i++)
            pkt.message[i] =  '0' + i;
        pkt.message[i] = 0;

        pkt.hdr.nChecksum = checksum(&pkt, sizeof(pkt));

        auto r = setsockopt(sock, IPPROTO_IP, 7, 
            (char*)&hop, sizeof(hop));

        if (r == SOCKET_ERROR)
        {
            DBG(
                printf("ERROR: %d\n", WSAGetLastError());
            )
        }
        


        dest.sin_family = AF_INET;
        dest.sin_port = htons(80);
        dest.sin_addr.s_addr = inet_addr(ip);

        auto begin = time(nullptr);

        int nResult = sendto(sock, (const char*)&pkt, sizeof(pkt), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
        //int nResult = sendto(sock, (const char*)&p, sizeof(p), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
        if (nResult == SOCKET_ERROR)
        {
            DBG(
                printf("ERROR: %d\n", WSAGetLastError());
            )
            WSACleanup();
        }

        int addr_len = sizeof(rtrn);

        char buffer[255];

        fd_set fd;
        FD_ZERO(&fd);

        TIMEVAL time;
        time.tv_sec = 10;
        time.tv_usec = 10;

        FD_SET(sock, &fd);

        /*if (select(sock, &fd, NULL, NULL, &time) == SOCKET_ERROR)
        {
            printf("Error: Invalid Socket (%d)\n", WSAGetLastError());
            return {};
        }*/

        Sleep(1000);


        //nResult = recvfrom(sock, buffer, sizeof(buffer), 0, (SOCKADDR*)&rtrn, & addr_len);
        nResult = recv(sock, buffer, sizeof(buffer), 0);
                
                result.push_back(rtrn.sin_addr);
                if (rtrn.sin_addr.s_addr == inet_addr(ip))
                    break;

        if (nResult == SOCKET_ERROR)
        {
            DBG(
                printf("ERROR: %d\n", WSAGetLastError());
            )
            WSACleanup();
        }

    }

    return result;
}
