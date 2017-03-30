#include <RobotListener_UDP.h>

namespace pt = boost::posix_time;

void RobotListener_UDP::receivedNewServoEvent(PushbotSignals &cmd) {
    int leftVelocity = cmd.left;
    int rightVelocity = cmd.right;

    // time since listening started, in ms
    pt::time_duration duration = pt::microsec_clock::local_time() - start;
    long timestamp = duration.total_milliseconds();

    // send log-line to listener
    send(std::to_string(timestamp) + "," + std::to_string(leftVelocity) 
         + "," + std::to_string(rightVelocity) + "\n");
}
