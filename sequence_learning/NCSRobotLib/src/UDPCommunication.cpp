#include <UDPCommunication.h>

UDPSender::UDPSender(boost::asio::ip::address_v4 receiver_ip,
                     int receiver_port)
: receiver_endpoint(receiver_ip, receiver_port),
  socket(io_service) {
    socket.open(udp::v4());
    buffer = "";
}

// close socket
UDPSender::~UDPSender() {
    flush();
    socket.close();
}

void UDPSender::send(std::string data) {
    // Check if buffer has enough space, otherwise flush
    if ((data.length() + buffer.length()) >= UDP_BUFFER_SIZE ) {
        flush();
    };

    // Append to buffer
    buffer += data;
}

void UDPSender::flush() {
    buffer.resize(UDP_BUFFER_SIZE);

    socket.send_to(boost::asio::buffer(buffer.c_str(), buffer.length()),
                   receiver_endpoint);
    buffer = "";
}
