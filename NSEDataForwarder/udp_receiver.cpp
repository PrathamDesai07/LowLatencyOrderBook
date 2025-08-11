#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

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

int main() {
    int port = 9000;  // Must match sender port
    int sockfd;
    struct sockaddr_in serverAddr{}, clientAddr{};
    socklen_t addrLen = sizeof(clientAddr);

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Bind to port
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        return -1;
    }

    std::cout << "Listening on UDP port " << port << "...\n";

    // Open CSV file
    std::ofstream outfile("data_received.csv");
    outfile << "MsgType,Timestamp,Open,High,Close,Low,Volume,Dividends,StockSplits,StreamID,SeqNo\n";

    while (true) {
        char buffer[sizeof(STREAM_HEADER) + sizeof(BAR_PAYLOAD)];
        ssize_t bytesReceived = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                                         (struct sockaddr*)&clientAddr, &addrLen);
        if (bytesReceived < 0) {
            perror("Receive failed");
            break;
        }

        // Extract structs
        STREAM_HEADER header;
        BAR_PAYLOAD payload;
        memcpy(&header, buffer, sizeof(STREAM_HEADER));
        memcpy(&payload, buffer + sizeof(STREAM_HEADER), sizeof(BAR_PAYLOAD));

        // Save to CSV
        outfile << payload.cMsgType << ","
                << payload.timestamp_ms << ","
                << payload.open << ","
                << payload.high << ","
                << payload.close << ","
                << payload.low << ","
                << payload.volume << ","
                << payload.dividends << ","
                << payload.stock_splits << ","
                << header.stream_id << ","
                << header.seq_no << "\n";

        outfile.flush(); // Write immediately
    }

    outfile.close();
    close(sockfd);
    return 0;
}
