#include "Robot.h"

// #define ECHO_ON


Robot::Robot(int fileDescriptor) {
    this->fileDescriptor = fileDescriptor;
    m_toggleListening = false;
    m_readThreadStatus = TERMINATE_THREAD;
    m_status = 0;
    count = new int;
    *count = 0;
    ios = new boost::asio::io_service();
    readTimer = new boost::asio::deadline_timer( *ios, boost::posix_time::milliseconds(UPDATE_TIME) );
    //readTimer->async_wait( boost::bind(threadLoop, readTimer, count, this) );
}

Robot::~Robot() {
    this->close();
}

int Robot::close(void)
{
  std::cout<<"> Closing Robot ..."<<std::endl;
  ::close(fileDescriptor);
  std::cout<<"Closed Serial Connection of the Robot"<<std::endl;
  return 0;
}

void Robot::registerListener(RobotListener* listener)
{
    m_listeners.push_back(listener);
}

void Robot::deregisterListener(RobotListener* listener)
{
    m_listeners.remove(listener);
}

int Robot::sendCommand(std::string cmd)
{
    unsigned int w = write(fileDescriptor,
                  cmd.c_str(),
                  cmd.length());
    #ifdef ECHO_ON
        std::string str = cmd.substr( 0, w);
        str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
        std::cout << "<< " << str << std::endl; // w - 1 to skip the \n at the end off the command
    #endif
    
    return !(w==cmd.length());
}

