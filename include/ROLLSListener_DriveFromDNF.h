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

	  ROLLSListener_DriveFromDNF(PushBot *robot,NeuronGroup dnf);

    virtual ~ROLLSListener_DriveFromDNF() {_stop = true; motor_thread.join();}

    void receivedNewROLLSEvent(unsigned int neuron);

    void setTurnFactor(double factor);
    
    void loop();

		void start();

		void stop();	

private:
    // Pushbot
    PushBot *pushbot;

    // Neuron Populations
    NeuronGroup DNF;

    //current heading direction of the robot
    double phi_curr;
		double tau_turn;
    double phi_tar;

    double speed;
		double delta_phi;
		double cameraOpeningAngle;

    // counter and states for spikes
    int turn_vel, forward_vel;

    // Scaling for speed and turning
    double speed_factor;
    double turn_factor;
		const int delta_t;

    // variables for motor thread
    std::thread motor_thread;
    std::mutex counter_mutex;
    bool _stop;

};

#endif
