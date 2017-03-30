#include <ROLLSListener_UDP.h>

void ROLLSListener_UDP::receivedNewROLLSEvent(unsigned int neuron)
{
    // time since listening started, in ms
    pt::time_duration duration = pt::microsec_clock::local_time() - start;
    long timestamp = duration.total_milliseconds();

    send(std::to_string(timestamp) + "," + std::to_string(neuron) + "\n");
}
