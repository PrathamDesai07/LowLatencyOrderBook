#include "UDPPacket.h"

double UDPPacket::safe_stod(const std::string &s) {
    try {
        return std::stod(s);
    } catch (...) {
        std::cerr << "Invalid double value: '" << s << "', defaulting to 0.0\n";
        return 0.0;
    }
}

uint64_t UDPPacket::safe_stoull(const std::string &s) {
    try {
        return std::stoull(s);
    } catch (...) {
        std::cerr << "Invalid uint64_t value: '" << s << "', defaulting to 0\n";
        return 0;
    }
}

uint64_t UDPPacket::current_timestamp_ms() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

STREAM_HEADER UDPPacket::header_generator(uint16_t stream_id, uint16_t seq_no) {
    STREAM_HEADER header;
    header.msg_len = sizeof(STREAM_HEADER) + sizeof(BAR_PAYLOAD);
    header.stream_id = stream_id;
    header.seq_no = seq_no;
    return header;
}

BAR_PAYLOAD UDPPacket::payload_generator(const std::vector<std::string> &dataVector) {
    BAR_PAYLOAD payload;
    payload.cMsgType = 'B';
    payload.open = safe_stod(dataVector[0]);
    payload.high = safe_stod(dataVector[1]);
    payload.close = safe_stod(dataVector[2]);
    payload.low = safe_stod(dataVector[3]);
    payload.volume = safe_stoull(dataVector[4]);
    payload.dividends = safe_stod(dataVector[5]);
    payload.stock_splits = safe_stod(dataVector[6]);
    payload.timestamp_ms = current_timestamp_ms();
    return payload;
}

std::vector<std::string> UDPPacket::data_spliter(std::string data) {
    std::vector<std::string> dataVector;
    std::stringstream ss(data);
    std::string item;
    while (std::getline(ss, item, ',')) {
        item.erase(std::remove_if(item.begin(), item.end(), ::isspace), item.end());
        dataVector.push_back(item);
    }
    return dataVector;
}

int UDPPacket::send_udp_packet(const STREAM_HEADER &header, const BAR_PAYLOAD &payload,
                               const std::string &ip, int port) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return -1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0) {
        perror("Invalid IP address");
        close(sockfd);
        return -1;
    }

    // Combine header and payload into a single buffer
    char buffer[sizeof(STREAM_HEADER) + sizeof(BAR_PAYLOAD)];
    memcpy(buffer, &header, sizeof(STREAM_HEADER));
    memcpy(buffer + sizeof(STREAM_HEADER), &payload, sizeof(BAR_PAYLOAD));

    ssize_t sent_bytes = sendto(sockfd, buffer, sizeof(buffer), 0,
                                (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (sent_bytes < 0) {
        perror("Send failed");
        close(sockfd);
        return -1;
    }

    close(sockfd);
    return 0;
}

int UDPPacket::UDPPacket_generator(std::string filepath, const std::string &ip, int port) {
    std::ifstream inputFile(filepath);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening the file!\n";
        return 1;
    }

    std::string line;
    uint16_t stream_id = 0;
    uint16_t seq_no = 0;
    bool isFirstLine = true;

    while (std::getline(inputFile, line)) {
        if (line.empty()) continue;
        if (isFirstLine) { isFirstLine = false; continue; }

        std::vector<std::string> dataVector = data_spliter(line);
        if (dataVector.size() < 7) {
            std::cerr << "Invalid row (less than 7 columns): " << line << "\n";
            continue;
        }
        stream_id++;
        seq_no++;
        STREAM_HEADER header = header_generator(stream_id, seq_no);
        BAR_PAYLOAD payload = payload_generator(dataVector);

        send_udp_packet(header, payload, ip, port);
    }
    return 0;
}
