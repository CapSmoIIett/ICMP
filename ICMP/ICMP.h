// ICMP.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>

#include "thread"

#include <iostream>
#include <random>
#include <sstream>


// TODO: Reference additional headers your program requires here.

struct ICMPheader
{
    unsigned char    byType;
    unsigned char    byCode;
    unsigned short   nChecksum;
    unsigned short   nId;
    unsigned short   nSequence;
};

// IPv4 header



#define ICMP_ECHOREPLY		0	/* Echo Reply			*/
#define ICMP_DEST_UNREACH	3	/* Destination Unreachable	*/
#define ICMP_SOURCE_QUENCH	4	/* Source Quench		*/
#define ICMP_REDIRECT		5	/* Redirect (change route)	*/
#define ICMP_ECHO		    8	/* Echo Request			*/
#define ICMP_TIME_EXCEEDED	11	/* Time Exceeded		*/
#define ICMP_PARAMETERPROB	12	/* Parameter Problem		*/
#define ICMP_TIMESTAMP		13	/* Timestamp Request		*/
#define ICMP_TIMESTAMPREPLY	14	/* Timestamp Reply		*/
#define ICMP_INFO_REQUEST	15	/* Information Request		*/
#define ICMP_INFO_REPLY		16	/* Information Reply		*/
#define ICMP_ADDRESS		17	/* Address Mask Request		*/
#define ICMP_ADDRESSREPLY	18	/* Address Mask Reply		*/
#define NR_ICMP_TYPES		18

#define PING_PKT_S 32             /* ping packet size */

struct ping_pkt {
    struct ICMPheader hdr;
    char message[PING_PKT_S - sizeof(ICMPheader)];
};


struct EthernetIIFrame
{
    uint64_t mac_dest : 48;       // Dest. MAC
    uint64_t mac_sour : 48;       // Source MAC
    uint16_t eth_type;            // Ethernet type
    
    
};

struct IPheader
{
    unsigned char  ip_verlen;        // 4-bit IPv4 version 4-bit header length (in 32-bit words)
    unsigned char  ip_tos;           // IP type of service
    unsigned short ip_totallength;   // Total length
    unsigned short ip_id;            // Unique identifier
    unsigned short ip_offset;        // Fragment offset field
    unsigned char  ip_ttl;           // Time to live
    unsigned char  ip_protocol;      // Protocol(TCP,UDP etc)
    unsigned short ip_checksum;      // IP checksum
    unsigned int   ip_srcaddr;       // Source address
    unsigned int   ip_destaddr;      // Source address

};


struct packet
{
    //EthernetIIFrame eth;
    IPheader ip;
    ping_pkt ping;
};


std::vector<in_addr> traceroute(char* ip);

void ping(char* ip, int amount_msg = 4);
void ping(SOCKET sock, char* ip, int& ids, int amount_msg = 4);

unsigned short checksum(void* buffer, int length);
uint32_t convert(const std::string& ipv4Str);

#ifdef  DEBUG	 
    #define DBG(exp) exp
#else
    #define DBG(exp) 
#endif