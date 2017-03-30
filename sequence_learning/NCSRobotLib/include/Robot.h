#ifndef ROBOT_H
#define ROBOT_H

#define UPDATE_TIME 1000

#include <queue>

#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include <RobotListener.h>


/*! \class Robot
 *  \brief Abstact base class defining the required interface to robot
 * 
 */
class Robot {
public:
     //! The constructor takes a valid file file descriptor
    Robot(int fileDescriptor);
    ~Robot();
    //! register a Listener to this robot
    void registerListener( RobotListener * );
    //! degregister a specific listener
    void deregisterListener( RobotListener * );
    //! start listening to new events from the data stream received
    int close();
    //! check the robot when starting up
    virtual bool twitch() = 0;
    //! process logged keystrokes and triggers the required action
    virtual void processKeys(int keysPressed) = 0;
    //! Parses the commands of interest from the input
    virtual void parse(unsigned char* data, int bytesRead) = 0; 
protected:

    // Threading for listening
    boost::thread*               readThread;
    int                          m_readThreadStatus;
    boost::asio::io_service*     ios;
    boost::asio::deadline_timer* readTimer;
    //static void                  threadLoop(boost::asio::deadline_timer* , int*, void*); //<! reads from the Serial device.
    static const int TERMINATE_THREAD = -1;
    static const int RUN_THREAD = 1;
    bool   m_toggleListening;
    
    std::list<RobotListener*> m_listeners; //!< Registered listeners

    int sendCommand(std::string cmd); //!< send a command to the device, is the same for a serial or a TCP device

    int m_status;
    int fileDescriptor; //!< File desciptor of the opened device. Needed for all interaction with the device.
     //! reads from the TCP device
    //void readSocket();

    // TODO try to get rid of
    int* count;
};

#endif
