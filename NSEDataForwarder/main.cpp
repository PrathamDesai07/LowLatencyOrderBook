#include "UDPPacket.h"

int main() {
    UDPPacket u;
    std::string filepath = "/teamspace/studios/this_studio/LowLatencyOrderBook/dataset/yf_dataset_AAPL.csv";
    std::string ip = "127.0.0.1"; // Change to target IP
    int port = 9000;              // Change to target port

    return u.UDPPacket_generator(filepath, ip, port);
}