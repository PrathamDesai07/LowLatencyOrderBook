#ifndef UDP_PACKET_H
#define UDP_PACKET_H

#include <iostream>
#include <unistd.h>
#include <cstdint>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <chrono>
#include <sstream>
#include <algorithm>

#pragma pack(push, 1)
struct STREAM_HEADER {
    uint16_t msg_len;
    uint16_t stream_id;
    uint16_t seq_no;
};

struct BAR_PAYLOAD {
    char cMsgType;
    uint64_t timestamp_ms;
    double open;
    double high;
    double close;
    double low;
    uint64_t volume;
    double dividends;
    double stock_splits;
};
#pragma pack(pop)

class UDPPacket {
public:
    static double safe_stod(const std::string &s);
    static uint64_t safe_stoull(const std::string &s);

    uint64_t current_timestamp_ms();
    STREAM_HEADER header_generator(uint16_t stream_id, uint16_t seq_no);
    BAR_PAYLOAD payload_generator(const std::vector<std::string> &dataVector);
    std::vector<std::string> data_spliter(std::string data);

    int UDPPacket_generator(std::string filepath, const std::string &ip, int port);
    int send_udp_packet(const STREAM_HEADER &header, const BAR_PAYLOAD &payload,
                        const std::string &ip, int port);
};

#endif
