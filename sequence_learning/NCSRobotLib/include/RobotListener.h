#ifndef ROBOTLISTENER_H
#define ROBOTLISTENER_H

/*! \file RobotListener.h
 *  \author Michel Frising, Hermann Blum
 * Provides an abstract base class to receive the servo states and the bumber states parsed from the robot event 
 * stream. 
 */

#include <string>
#include <queue>

// this is for omnibot
struct servoSignals {
    int values[3];
    std::string timestamp;
};

struct PushbotSignals {
    int left;
    int right;
};

struct bumper {
    int value;
    std::string timestamp;
};

//! Abstarct base class providing the necessary interface to observe the servo states and bumper states of the observed robot
/*!
 * This class needs to be subcalssed and the member functions need to be implemented in order to use the interface
 */
class RobotListener 
{
    public:
        RobotListener() {}
        virtual void receivedNewServoEvent(PushbotSignals &cmd) = 0;
};

#endif
