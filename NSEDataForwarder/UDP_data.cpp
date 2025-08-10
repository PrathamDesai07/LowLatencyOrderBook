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
using namespace std;

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
    static double safe_stod(const string &s) {
        try {
            return stod(s);
        } catch (...) {
            cerr << "Invalid double value: '" << s << "', defaulting to 0.0\n";
            return 0.0;
        }
    }

    static uint64_t safe_stoull(const string &s) {
        try {
            return stoull(s);
        } catch (...) {
            cerr << "Invalid uint64_t value: '" << s << "', defaulting to 0\n";
            return 0;
        }
    }
    uint64_t current_timestamp_ms() {
        using namespace std::chrono;
        return duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()
        ).count();
    }
    STREAM_HEADER header_generator(uint16_t stream_id, uint16_t seq_no) {
        STREAM_HEADER header;
        header.msg_len = sizeof(STREAM_HEADER) + sizeof(BAR_PAYLOAD);
        header.stream_id = stream_id;
        header.seq_no = seq_no;
        return header;
    }

    BAR_PAYLOAD payload_generator(const vector<string> &dataVector) {
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

    vector<string> data_spliter(string data) {
        vector<string> dataVector;
        stringstream ss(data);
        string item;
        while (getline(ss, item, ',')) {
            // Trim spaces
            item.erase(remove_if(item.begin(), item.end(), ::isspace), item.end());
            dataVector.push_back(item);
        }
        return dataVector;
    }

    int UDPPacket_generator(string filepath) {
        ifstream inputFile(filepath);
        if (!inputFile.is_open()) {
            cerr << "Error opening the file!\n";
            return 1;
        }

        string line;
        uint16_t stream_id = 0;
        uint16_t seq_no = 0;
        bool isFirstLine = true; // Skip CSV header

        while (getline(inputFile, line)) {
            if (line.empty()) continue; // skip blank lines
            if (isFirstLine) { isFirstLine = false; continue; } // skip header

            vector<string> dataVector = data_spliter(line);
            if (dataVector.size() < 7) {
                cerr << "Invalid row (less than 7 columns): " << line << "\n";
                continue;
            }
            stream_id++;
            seq_no++;
            STREAM_HEADER header = header_generator(stream_id, seq_no);
            BAR_PAYLOAD payload = payload_generator(dataVector);
        }
        return 0;
    }
};

int main() {
    UDPPacket u;
    return u.UDPPacket_generator("/teamspace/studios/this_studio/LowLatencyOrderBook/dataset/yf_dataset_AAPL.csv");
}
