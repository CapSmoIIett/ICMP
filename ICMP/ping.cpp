
#include "ICMP.h"

void ping(char* ip, int amount_msg)
{
    int i = 0;
    int bytes = 0;
    int received = 0;
    int nSequence = 0;
    unsigned int ttl_val = 56;

    SOCKET sock;
    WSADATA wsa;
    struct sockaddr_in dest;
    struct sockaddr_in rtrn;

    packet recv;
    ping_pkt pkt;

    
    time_t t_min = 0xFFF;
    time_t t_max = 0;
    time_t t_av = 0;

    srand(time(NULL));

    WSAStartup(MAKEWORD(2, 2), &wsa);
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    setsockopt(sock, IPPROTO_IP, IP_TTL,
        (char*)&ttl_val, sizeof(ttl_val));

    std::cout << "Pinging " << ip << " with " << sizeof(ping_pkt) << " bytes of data:" << "\n\n";

    for (int it = 0; it < amount_msg; it++)
    {
        //Sequence number is incremented with each request, this should be different
        //so we can match a response with a request
        pkt.hdr.byCode = 0;// ICMP_ECHO;       //Zero for ICMP echo and reply messages
        pkt.hdr.nSequence = htons(nSequence++);
        pkt.hdr.byType = ICMP_ECHO;       //Eight for ICMP echo message
        pkt.hdr.nChecksum = 0;    //Checksum is calculated later ono 
        pkt.hdr.nId = GetCurrentProcessId();


        for (i = 0; i < PING_PKT_S - sizeof(ICMPheader) - 1; i++)
            pkt.message[i] = rand() % 10 + '0';
        pkt.message[i] = 0;

        pkt.hdr.nChecksum = checksum(&pkt, sizeof(pkt));


        dest.sin_family = AF_INET;
        dest.sin_port = htons(0);
        dest.sin_addr.s_addr = inet_addr(ip);

        auto begin = time(nullptr);

        int nResult = sendto(sock, (const char*)&pkt, sizeof(pkt), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
        if (nResult == SOCKET_ERROR)
        {
            DBG(
                printf("ERROR: %d\n", WSAGetLastError());
            )
            WSACleanup();
        }

        int addr_len = sizeof(rtrn);
        nResult = recvfrom(sock, (char*)&recv, sizeof(recv), 0, (SOCKADDR*)&rtrn, &addr_len);

        if (nResult == SOCKET_ERROR)
        {
            DBG(
                printf("ERROR: %d\n", WSAGetLastError());
            )
            WSACleanup();
        }

        auto end = time(nullptr);

        auto time = end - begin;

        if (time < t_min) t_min = time;
        if (time > t_max) t_max = time;
        t_av = (t_av + time) / 2;

        std::cout << "Reply from " << ip << ": bytes=" << sizeof(ping_pkt) << " time=" << time << "ms " << "TTL=" << ttl_val << "\n";

        for (i = 0; i < PING_PKT_S - sizeof(ICMPheader); i++)
            if (pkt.message[i] == recv.ping.message[i])
                received++;
    }

    std::cout << "\n" << "Ping statistics for " << ip << "\n";
    std::cout << "\t" << "Packets: " << "Sent = " << amount_msg << ", Received = " << received / (PING_PKT_S - sizeof(ICMPheader))
            << ", Lost = " << (PING_PKT_S - sizeof(ICMPheader)) * amount_msg - received << " (" << ((PING_PKT_S - sizeof(ICMPheader)) * amount_msg - received) / 100<< "% loss)," << "\n";
    std::cout << "Approximate round trip in milli-seconds" << "\n";
    std::cout << "\t" << "Minimum = " << t_min << "ms, Maximum = " << t_max << "ms, Average = " << t_av <<  "ms" << "\n";

}


void ping(SOCKET sock, char* ip, int& id, int amount_msg)
{
    int i = 0;
    int bytes = 0;
    int received = 0;
    int nSequence = 0;
    unsigned int ttl_val = 56;

    WSADATA wsa;
    struct sockaddr_in dest;
    struct sockaddr_in rtrn;

    packet recv;
    ping_pkt pkt;

    
    time_t t_min = 0xFFF;
    time_t t_max = 0;
    time_t t_av = 0;

    srand(time(NULL));

    std::cout << "Pinging " << ip << " with " << sizeof(ping_pkt) << " bytes of data:" << "\n\n";

    for (int it = 0; it < amount_msg; it++)
    {
        int pid = id++;
        //Sequence number is incremented with each request, this should be different
        //so we can match a response with a request
        pkt.hdr.byCode = 0;       //Zero for ICMP echo and reply messages
        pkt.hdr.nSequence = htons(nSequence++);
        pkt.hdr.byType = ICMP_ECHO;       //Eight for ICMP echo message
        pkt.hdr.nChecksum = 0;    //Checksum is calculated later ono 
        pkt.hdr.nId = GetCurrentProcessId();


        pkt.message[0] = pid;
        for (i = 1; i < PING_PKT_S - sizeof(ICMPheader) - 1; i++)
            pkt.message[i] = '0' + i;// rand() % 10 + '0';
        pkt.message[i] = 0;

        pkt.hdr.nChecksum = checksum(&pkt, sizeof(pkt));

        dest.sin_family = AF_INET;
        dest.sin_port = htons(0);
        dest.sin_addr.s_addr = inet_addr(ip);

        auto begin = time(nullptr);

        int nResult = sendto(sock, (const char*)&pkt, sizeof(pkt), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
        if (nResult == SOCKET_ERROR)
        {
            DBG(
                printf("ERROR: %d\n", WSAGetLastError());
            )
            WSACleanup();
        }

        timeval timer;
        timer.tv_sec = 2;
        timer.tv_usec = 0;

        fd_set setSocket;
        FD_ZERO(&setSocket);
        FD_SET(sock, &setSocket);

        while (true)
        {
            auto result = select(NULL, &setSocket, NULL, NULL, &timer);

            int addr_len = sizeof(rtrn);
            nResult = recvfrom(sock, (char*)&recv, sizeof(recv), MSG_PEEK, (SOCKADDR*)&rtrn, &addr_len);

            if (nResult == SOCKET_ERROR)
            {
                DBG(
                    printf("ERROR: %d\n", WSAGetLastError());
                )
                WSACleanup();
            }

            if (recv.ping.message[0] != pid)
                continue;

            nResult = recvfrom(sock, (char*)&recv, sizeof(recv), NULL, (SOCKADDR*)&rtrn, &addr_len);

            //if (inet_ntoa((in_addr)recv.ip.ip_srcaddr) == convert(ip))
            //if (recv.ip.ip_srcaddr == inet_addr(ip))
            break;
        }

        auto end = time(nullptr);

        auto time = end - begin;

        if (time < t_min) t_min = time;
        if (time > t_max) t_max = time;
        t_av = (t_av + time) / 2;

        std::cout << "Reply from " << ip << ": bytes=" << sizeof(ping_pkt) << " time=" << time << "ms " << "TTL=" << ttl_val << "\n";

        for (i = 0; i < PING_PKT_S - sizeof(ICMPheader); i++)
        {
            if (pkt.message[i] == recv.ping.message[i])
                received++;
            else
                std::cout << pid << "x\n";
        }
    }

    std::cout << "\n" << "Ping statistics for " << ip << "\n";
    std::cout << "\t" << "Packets: " << "Sent = " << amount_msg
        << ", Received = " << static_cast<double>(received) / (PING_PKT_S - sizeof(ICMPheader))
        << ", Lost = " << static_cast<double>(PING_PKT_S - sizeof(ICMPheader)) * amount_msg - received << " (" << ((PING_PKT_S - sizeof(ICMPheader)) * amount_msg - received) / 100<< "% loss)," << "\n";
    std::cout << "Approximate round trip in milli-seconds" << "\n";
    std::cout << "\t" << "Minimum = " << t_min << "ms, Maximum = " << t_max << "ms, Average = " << t_av <<  "ms" << "\n";

    
}

uint32_t convert(const std::string& ipv4Str)
{
    std::istringstream iss(ipv4Str);

    uint32_t ipv4 = 0;

    for (uint32_t i = 0; i < 4; ++i) {
        uint32_t part;
        iss >> part;
        if (iss.fail() || part > 255) {
            throw std::runtime_error("Invalid IP address - Expected [0, 255]");
        }

        // LSHIFT and OR all parts together with the first part as the MSB
        ipv4 |= part << (8 * (3 - i));

        // Check for delimiter except on last iteration
        if (i != 3) {
            char delimiter;
            iss >> delimiter;
            if (iss.fail() || delimiter != '.') {
                throw std::runtime_error("Invalid IP address - Expected '.' delimiter");
            }
        }
    }

    return ipv4;
}


unsigned short checksum(void* buffer, int length)
{
    unsigned short* buff = (unsigned short*)buffer;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; length > 1; length -= 2)
        sum += *buff++;

    if (length == 1)
        sum += *(unsigned char*)buff;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}


