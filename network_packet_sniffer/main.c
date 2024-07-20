#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <net/ethernet.h>

void process_packet(const u_char *, struct pcap_pkthdr *, const u_char *);
void print_ethernet_header(const u_char *, int);
void print_ip_header(const u_char *, int);
void print_tcp_packet(const u_char *, int);
void print_udp_packet(const u_char *, int);
void print_data(const u_char *, int);

FILE *logfile;
struct sockaddr_in source, dest;

int main()
{
    pcap_t *handle;
    char errbuf[PCAP_ERRBUF_SIZE];
    struct pcap_pkthdr header;
    const u_char *packet;
    char dev[] = "eth0";

    logfile = fopen("packet_log.txt", "w");
    if (logfile == NULL)
    {
        printf("Unable to create file.\n");
        return 1;
    }

    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL)
    {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        return 2;
    }

    pcap_loop(handle, -1, process_packet, NULL);

    pcap_close(handle);
    fclose(logfile);

    return 0;
}

void process_packet(const u_char *packet, struct pcap_pkthdr *header, const u_char *userdata)
{
    fprintf(logfile, "\n\n========= Packet =========\n");

    print_ethernet_header(packet, header->len);

    struct ip *ip_header = (struct ip *)(packet + sizeof(struct ether_header));
    switch (ip_header->ip_p)
    {
    case IPPROTO_TCP:
        print_tcp_packet(packet, header->len);
        break;

    case IPPROTO_UDP:
        print_udp_packet(packet, header->len);
        break;

    default:
        fprintf(logfile, "Other Protocol\n");
        break;
    }
}

void print_ethernet_header(const u_char *Buffer, int Size)
{
    struct ether_header *eth = (struct ether_header *)Buffer;

    fprintf(logfile, "\nEthernet Header\n");
    fprintf(logfile, "   |-Source Address      : %02x:%02x:%02x:%02x:%02x:%02x \n",
            eth->ether_shost[0], eth->ether_shost[1], eth->ether_shost[2],
            eth->ether_shost[3], eth->ether_shost[4], eth->ether_shost[5]);
    fprintf(logfile, "   |-Destination Address : %02x:%02x:%02x:%02x:%02x:%02x \n",
            eth->ether_dhost[0], eth->ether_dhost[1], eth->ether_dhost[2],
            eth->ether_dhost[3], eth->ether_dhost[4], eth->ether_dhost[5]);
    fprintf(logfile, "   |-Protocol            : %u \n", (unsigned short)eth->ether_type);
}

void print_ip_header(const u_char *Buffer, int Size)
{
    print_ethernet_header(Buffer, Size);
    unsigned short iphdrlen;

    struct ip *ip_header = (struct ip *)(Buffer + sizeof(struct ether_header));
    iphdrlen = ip_header->ip_hl * 4;

    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = ip_header->ip_src.s_addr;

    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = ip_header->ip_dst.s_addr;

    fprintf(logfile, "\nIP Header\n");
    fprintf(logfile, "   |-IP Version        : %d\n", (unsigned int)ip_header->ip_v);
    fprintf(logfile, "   |-IP Header Length  : %d DWORDS or %d Bytes\n", (unsigned int)ip_header->ip_hl, ((unsigned int)(ip_header->ip_hl)) * 4);
    fprintf(logfile, "   |-Type Of Service   : %d\n", (unsigned int)ip_header->ip_tos);
    fprintf(logfile, "   |-IP Total Length   : %d  Bytes(Size of Packet)\n", ntohs(ip_header->ip_len));
    fprintf(logfile, "   |-Identification    : %d\n", ntohs(ip_header->ip_id));
    fprintf(logfile, "   |-TTL      : %d\n", (unsigned int)ip_header->ip_ttl);
    fprintf(logfile, "   |-Protocol : %d\n", (unsigned int)ip_header->ip_p);
    fprintf(logfile, "   |-Checksum : %d\n", ntohs(ip_header->ip_sum));
    fprintf(logfile, "   |-Source IP        : %s\n", inet_ntoa(source.sin_addr));
    fprintf(logfile, "   |-Destination IP   : %s\n", inet_ntoa(dest.sin_addr));
}

void print_tcp_packet(const u_char *Buffer, int Size)
{
    unsigned short iphdrlen;

    struct ip *ip_header = (struct ip *)(Buffer + sizeof(struct ether_header));
    iphdrlen = ip_header->ip_hl * 4;

    struct tcphdr *tcp_header = (struct tcphdr *)(Buffer + iphdrlen + sizeof(struct ether_header));

    fprintf(logfile, "\nTCP Packet\n");
    print_ip_header(Buffer, Size);

    fprintf(logfile, "\nTCP Header\n");
    fprintf(logfile, "   |-Source Port      : %u\n", ntohs(tcp_header->source));
    fprintf(logfile, "   |-Destination Port : %u\n", ntohs(tcp_header->dest));
    fprintf(logfile, "   |-Sequence Number    : %u\n", ntohl(tcp_header->seq));
    fprintf(logfile, "   |-Acknowledge Number : %u\n", ntohl(tcp_header->ack_seq));
    fprintf(logfile, "   |-Header Length      : %d DWORDS or %d BYTES\n", (unsigned int)tcp_header->doff, (unsigned int)tcp_header->doff * 4);
    fprintf(logfile, "   |-Urgent Flag          : %d\n", (unsigned int)tcp_header->urg);
    fprintf(logfile, "   |-Acknowledgement Flag : %d\n", (unsigned int)tcp_header->ack);
    fprintf(logfile, "   |-Push Flag            : %d\n", (unsigned int)tcp_header->psh);
    fprintf(logfile, "   |-Reset Flag           : %d\n", (unsigned int)tcp_header->rst);
    fprintf(logfile, "   |-Synchronise Flag     : %d\n", (unsigned int)tcp_header->syn);
    fprintf(logfile, "   |-Finish Flag          : %d\n", (unsigned int)tcp_header->fin);
    fprintf(logfile, "   |-Window         : %d\n", ntohs(tcp_header->window));
    fprintf(logfile, "   |-Checksum       : %d\n", ntohs(tcp_header->check));
    fprintf(logfile, "   |-Urgent Pointer : %d\n", tcp_header->urg_ptr);
    fprintf(logfile, "\n");
    fprintf(logfile, "                        DATA Dump                         ");
    fprintf(logfile, "\n");

    fprintf(logfile, "IP Header\n");
    print_data(Buffer, iphdrlen);

    fprintf(logfile, "TCP Header\n");
    print_data(Buffer + iphdrlen, tcp_header->doff * 4);

    fprintf(logfile, "Data Payload\n");
    print_data(Buffer + iphdrlen + tcp_header->doff * 4, (Size - tcp_header->doff * 4 - iphdrlen));

    fprintf(logfile, "\n###########################################################");
}

void print_udp_packet(const u_char *Buffer, int Size)
{
    unsigned short iphdrlen;

    struct ip *ip_header = (struct ip *)(Buffer + sizeof(struct ether_header));
    iphdrlen = ip_header->ip_hl * 4;

    struct udphdr *udp_header = (struct udphdr *)(Buffer + iphdrlen + sizeof(struct ether_header));

    fprintf(logfile, "\nUDP Packet\n");
    print_ip_header(Buffer, Size);

    fprintf(logfile, "\nUDP Header\n");
    fprintf(logfile, "   |-Source Port      : %d\n", ntohs(udp_header->source));
    fprintf(logfile, "   |-Destination Port : %d\n", ntohs(udp_header->dest));
    fprintf(logfile, "   |-UDP Length       : %d\n", ntohs(udp_header->len));
    fprintf(logfile, "   |-UDP Checksum     : %d\n", ntohs(udp_header->check));

    fprintf(logfile, "\n");

    fprintf(logfile, "IP Header\n");
    print_data(Buffer, iphdrlen);

    fprintf(logfile, "UDP Header\n");
    print_data(Buffer + iphdrlen, sizeof(udp_header));

    fprintf(logfile, "Data Payload\n");

    print_data(Buffer + iphdrlen + sizeof(udp_header), (Size - sizeof(udp_header) - iphdrlen));

    fprintf(logfile, "\n###########################################################");
}

void print_data(const u_char *data, int Size)
{
    int i, j;
    for (i = 0; i < Size; i++)
    {
        if (i != 0 && i % 16 == 0)
        {
            fprintf(logfile, "         ");
            for (j = i - 16; j < i; j++)
            {
                if (data[j] >= 32 && data[j] <= 128)
                    fprintf(logfile, "%c", (unsigned char)data[j]); // if its a number or alphabet
                else
                    fprintf(logfile, "."); // otherwise print a dot
            }
            fprintf(logfile, "\n");
        }

        if (i % 16 == 0)
            fprintf(logfile, "   ");
        fprintf(logfile, " %02X", (unsigned int)data[i]);

        if (i == Size - 1)
        {
            for (j = 0; j < 15 - i % 16; j++)
            {
                fprintf(logfile, "   ");
            }
            fprintf(logfile, "         ");

            for (j = i - i % 16; j <= i; j++)
            {
                if (data[j] >= 32 && data[j] <= 128)
                {
                    fprintf(logfile, "%c", (unsigned char)data[j]);
                }
                else
                {
                    fprintf(logfile, ".");
                }
            }
            fprintf(logfile, "\n");
        }
    }
}
