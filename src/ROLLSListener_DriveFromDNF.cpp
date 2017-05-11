#include <ROLLSListener_DriveFromDNF.h>
#include <chrono>  // milliseconds

// Future improvement: Let this be specified in constructor
const int INTERVAL = 500;  // time in milliseconds to check firing counts
//const double WEIGHT_OLD = 0.6;  // Factor for exponential window
//const double WEIGHT_NEW = 0.4;  // Factor for exponential window


ROLLSListener_DriveFromDNF::ROLLSListener_DriveFromDNF(PushBot *robot,NeuronGroup dnf)
    : pushbot(robot), DNF(dnf), delta_t(50), motor_thread(std::bind(&ROLLSListener_DriveFromDNF::loop, this)) 
{
	phi_curr = 0;
	phi_tar = 0;
	speed_factor = 0;
	turn_factor = 1;
	tau_turn = 100;
	delta_phi = 0;
	cameraOpeningAngle = 60;
	_stop = true;
};

void ROLLSListener_DriveFromDNF::receivedNewROLLSEvent(unsigned int event) 
{
    counter_mutex.lock();
    if (DNF.isOrigin(event)) 
	    phi_tar = ( event - DNF.start - DNF.size() / 2 ) * cameraOpeningAngle / DNF.size();
		std::cout<<"event = "<<event<<"DNF.start = "<<DNF.start<<"DNF.size="<<DNF.size()<<"OpeningAngle="<<cameraOpeningAngle<<"phi_tar = "<<phi_tar<<std::endl;	
    counter_mutex.unlock();
}

void ROLLSListener_DriveFromDNF::setTurnFactor(double factor)
{
	turn_factor = factor;
}

void ROLLSListener_DriveFromDNF::start()
{
	if (_stop)
		_stop = false;
	std::cout<<"DriveFromDNF: starting thread"<<std::endl;
	loop();
}


void ROLLSListener_DriveFromDNF::stop()
{
	if (!_stop)
		_stop = true;
	std::cout<<"DriveFromDNF: stop thread"<<std::endl;
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
	  std::cout<<"DriveFromDNF loop, outside stop "<<std::endl;
    if (!_stop) 
    {
			  std::cout<<"DriveFromDNF loop, inside stop: phi_tar= "<<phi_tar<<"phi_curr= "<<phi_curr<<std::endl;
				if ( abs(phi_tar - phi_curr) > 0.5 )
				{
          counter_mutex.lock();
					delta_phi = (delta_t / tau_turn) * turn_factor * (phi_curr-phi_tar);
					forward_vel = (delta_t / tau_turn) * speed * speed_factor;
					std::cout<<"Going to drive with forwadr_vel =  "<<forward_vel<<", delta_phi ="<<delta_phi<<std::endl;
				  counter_mutex.unlock();
					pushbot->move(forward_vel, delta_phi / delta_t);
					std::this_thread::sleep_for(std::chrono::milliseconds(delta_t));
					//pushbot->move(0, 0);
					phi_curr = phi_curr + delta_phi;	
				}
    }
}
