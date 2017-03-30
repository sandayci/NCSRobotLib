#include <PlasticSynapses.h>
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>
#include <unistd.h>
#include <ROLLSListener_Log.h>

void PlasticSynapses::readSynapses(ROLLSDevice& rolls, int start_neuron, int end_neuron) {

	// find local time      
	boost::posix_time::ptime start_t = boost::posix_time::microsec_clock::local_time();
	
	std::cout << "log files are set up" << std::endl;
	std::string log_dir = "../logs";
	std::string exp_name = "plastic_syn";

	int diff = 0;
	// stimulate sync neuron with virtual synapses to start the readout, 700 ms
	std::cout  << "Start stimulating" << std::endl;

	while (diff < 700) {
                boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
                boost::posix_time::time_duration diff2 = now - start_t;
                diff =  diff2.total_milliseconds();

		rolls.stimulate(200);
		usleep(32);
	}
	std::cout  << "Done stimulating the sync neuron" << std::endl;

	usleep(800000);
	// stimulate plastic syanpses of all neurons from 1 to 256 in different time windows
	std::cout  << "Start stimulating all neurons" << std::endl;
	for (int i = start_neuron; i < end_neuron+1; i++) {

		boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
		diff = 0;
				
		// for 50 ms, changed to 500 
  		while (diff < 300) {
			boost::posix_time::ptime now_2 = boost::posix_time::microsec_clock::local_time();
                	boost::posix_time::time_duration diff2 = now_2 - now;
                	diff = diff2.total_milliseconds();
			// stimulate all 256 at the ith plastic synapse
			//for (int j = 0; j < 256; j++) {
			//rolls.stimplastic(j, i);
			//}
			
			// stimulate neurons to test (from start to end neuron)
		  	rolls.stimulate(i);
			usleep(1.2);
		}
		while (diff>300 && diff< 150000) {
			for (int j = 0; j < 256; j++) {
			boost::posix_time::ptime now_2 = boost::posix_time::microsec_clock::local_time();
	      		boost::posix_time::time_duration diff2 = now_2 - now;
	      		diff = diff2.total_milliseconds();
			rolls.stimulate(j,4),
			rolls.stimulate(j,5);
			rolls.stimulate(j,6);
			rolls.stimulate(j,7);
			}
			usleep(0.4);
		}
		usleep(50000);
	}
	std::cout  << "Done stimulating neurons for readout plastic" << std::endl;
	
	// stimulate sync neuron with end spikes
	boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();        
	diff = 0;
	while (diff < 700) {
                boost::posix_time::ptime now_2 = boost::posix_time::microsec_clock::local_time();
                boost::posix_time::time_duration diff2 = now_2 - now;
                diff = diff2.total_milliseconds();
                rolls.stimulate(200);
                usleep(1.666);
        }
	std::cout  << "Done stimulating sync end neuron" << std::endl;
	std::cout  << "ROLLS stimulation done" << std::endl;
	std::terminate();
		
  }

