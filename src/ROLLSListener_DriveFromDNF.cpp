#include <ROLLSListener_DriveFromDNF.h>
#include <chrono>  // milliseconds

// Future improvement: Let this be specified in constructor
const int INTERVAL = 500;  // time in milliseconds to check firing counts
//const double WEIGHT_OLD = 0.6;  // Factor for exponential window
//const double WEIGHT_NEW = 0.4;  // Factor for exponential window



void ROLLSListener_DriveFromDNF::receivedNewROLLSEvent(unsigned int event) 
{
    counter_mutex.lock();
    if (DNF.isOrigin(event)) 
	    phi_tar = event - DNF.start - DNF.size() / 2; 
    counter_mutex.unlock();
}

void ROLLSListener_DriveFromDNF::goNoGo(bool stop)
{
	_stop = stop;
}

void ROLLSListener_DriveFromDNF::setTurnFactor(double factor)
{
	turn_factor = factor;
}

void ROLLSListener_DriveFromDNF::start()
{
	if (!thread_running)
	{
		motor_thread = std::thread(std::bind(&ROLLSListener_DriveFromDNF::loop, this));
		thread_running = true;
		std::cout<<"DriveFromDNF: starting thread"<<std::endl;
	}
}


void ROLLSListener_DriveFromDNF::stop()
{
	thread_running = false;
	motor_thread.join();
	std::cout<<"DriveFromDNF: stop thread"<<std::endl;
}


void ROLLSListener_DriveFromDNF::loop() 
{
    // Every INTERVAL time, update motors, then sleep for remaining time
    // Note: Execution takes less than a millisecond, so we don't time anything
    while (!_stop) 
    {
        counter_mutex.lock();
	if ( abs(phi_tar - phi_curr) > 0.5 )
	{
		time_factor = 1000 / INTERVAL;
		turn_vel = turn_factor * time_factor * (phi_tar-phi_curr);
		forward_vel = speed * speed_factor * time_factor;
	        pushbot->move(forward_vel, turn_vel);
		std::this_thread::sleep_for(std::chrono::milliseconds(INTERVAL));
	        pushbot->move(0, 0);
		phi_curr = phi_curr + turn_vel * INTERVAL;	
	}
        counter_mutex.unlock();
    }
}
