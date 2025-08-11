CXX = g++
CXXFLAGS = -Wall -O2

# Executable names
SENDER = sender
RECEIVER = receiver

# Source files
SENDER_SRC = main.cpp UDPPacket.cpp
RECEIVER_SRC = udp_receiver.cpp

# Default target: build both
all: $(SENDER) $(RECEIVER)

$(SENDER): $(SENDER_SRC) UDPPacket.h
	$(CXX) $(CXXFLAGS) $(SENDER_SRC) -o $(SENDER)

$(RECEIVER): $(RECEIVER_SRC)
	$(CXX) $(CXXFLAGS) $(RECEIVER_SRC) -o $(RECEIVER)

run-sender: $(SENDER)
	./$(SENDER)

run-receiver: $(RECEIVER)
	./$(RECEIVER)

clean:
	rm -f $(SENDER) $(RECEIVER) data_received.csv
