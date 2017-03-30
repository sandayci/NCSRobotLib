#include <RobotListener_Log.h>

void RobotListener_Log::receivedNewServoEvent(PushbotSignals &cmd) 
{
    // time since listening started, in ms
    pt::time_duration duration = pt::microsec_clock::local_time() - start;
    timestamp = duration.total_milliseconds();

    fprintf(log, "%ld,%d,%d\n",
            timestamp, cmd.left, cmd.right);
}
