#ifndef __ROLLS_LISTENER_DRIVE_FROM_DNF_H__
#define __ROLLS_LISTENER_DRIVE_FROM_DNF_H__

#include <ROLLSListener.h>
#include <ROLLSDevice.h>  // NeuronGroup
#include <PushBot.h>
#include <thread>
#include <mutex>
#include <functional>  // bind

class ROLLSListener_DriveFromDNF : public ROLLSListener 
{
public:
    ROLLSListener_DriveFromDNF(PushBot *robot,NeuronGroup dnf)
    : pushbot(robot), DNF(dnf)  
    {
	phi_curr = 2;
	phi_tar = 0;
	speed_factor = 0;
	turn_factor = 1;
    };

    virtual ~ROLLSListener_DriveFromDNF() {_stop = true; motor_thread.join();}

    void receivedNewROLLSEvent(unsigned int neuron);

    void goNoGo(bool stop);

    void setTurnFactor(double factor);

    void start();

    void stop();
    
private:
    // Pushbot
    PushBot *pushbot;
    void loop();

    // Neuron Populations
    NeuronGroup DNF;

    //current heading direction of the robot
    int phi_curr;

    //target heading direction, set by activity peak in the DNF
    int phi_tar;

    //turning command towards the target heading direction
    int speed;

    // counter and states for spikes
    int turn_vel, forward_vel;

    // Scaling for speed and turning
    double speed_factor;
    double turn_factor;
    double time_factor;

    bool thread_running;
    // variables for motor thread
    std::thread motor_thread;
    std::mutex counter_mutex;
    bool _stop = true;

};

#endif
