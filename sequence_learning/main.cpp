#include <iostream>
#include <PushBot.h>
#include <EDVSDevice.h>
#include <ROLLSDevice.h>
#include <ROLLSListener_Log.h>
#include <RobotListener_Log.h>
#include <EDVSListener_Log.h>
#include <TerminalInputDevice.h>
#include <boost/program_options.hpp>
#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>
#include <unistd.h>
#include <TCPConnector.h>
#include <Enums.h>

#include <signal.h>  // signals for program abort
volatile sig_atomic_t flag = 0;
void aborter(int sig){ // can be called asynchronously
  flag = 1; // set flag
}

#include <SequenceLearning.h>
namespace pt = boost::posix_time;

namespace po = boost::program_options;

int loop(int robotFileDescriptor, int dvsFileDescriptor, bool setup, bool reset, bool enable_logging, std::string eDVS_log, std::string ROLLS_log, std::string robot_log, std::string eDVS_IMU_log) {

    // start time for all the loggers
    pt::ptime start = pt::microsec_clock::local_time();

    // Pushbot
    PushBot robot(robotFileDescriptor);
    robot.enableMotors();
	  
    std::cout<<"robot initialized"<<std::endl;

    // eDVS
     EDVSDevice eDVSdevice(dvsFileDescriptor);
    if( eDVSdevice.isInit() < 0 ) {
        std::cout<<"eDVS device could not be initialised"<<std::endl;
        return -1;
    }
    eDVSdevice.changeEventFormat(EVT_FORMAT_E3);
    std::cout<<"DVS initialized"<<std::endl;

    // ROLLS and a udp listener
    // bad neurons are 170, 173, 22, 1122
    std::list<int> blacklist;
    blacklist.push_back(170);
    blacklist.push_back(173);
    blacklist.push_back(22);
    blacklist.push_back(112);
    ROLLSDevice *rolls = new ROLLSDevice(blacklist);
    ROLLSDevice *rolls_new = new ROLLSDevice(blacklist);
		
    //Keyboard Listener
    TerminalInput terminput;

    // Set up logs for all three      
   // Important! Create them in the main scope     
     RobotListener_Log *robot_logger;     
     EDVSListener_Log *edvs_logger;     
     EDVSIMUListener_Log *edvs_imu_logger;     
     ROLLSListener_Log *rolls_logger;     
     if (enable_logging) {         
         robot_logger = new RobotListener_Log(robot_log, start);         
         edvs_logger = new EDVSListener_Log(eDVS_log, start);         
         edvs_imu_logger = new EDVSIMUListener_Log(eDVS_IMU_log, start);         
         rolls_logger = new ROLLSListener_Log(ROLLS_log, start);         
         robot.registerListener(robot_logger);         
         eDVSdevice.registerListener(edvs_logger);         
         eDVSdevice.registerListener(edvs_imu_logger);         
          rolls->registerListener(rolls_logger);    
 	} 

    // Controller
    SequenceLearning controller(rolls, &robot, &eDVSdevice);
		SequenceLearning reset_rolls(rolls_new, &robot, &eDVSdevice);

    // If needed, apply the controllers ROLLS architecture
    if (setup) {
        std::cout<<"Synapse setup started."<<std::endl;
        controller.apply(*rolls);
        std::cout<<"Synapse setup done."<<std::endl;
    }

    // If needed, reset ROLLS architecture
    if(reset) {
	std::cout<<"Reset setup started."<<std::endl;
	reset_rolls.apply(*rolls_new);
	std::cout<<"Reset setup done."<<std::endl;
     }

    // Finally register the controller as listener
    rolls->registerListener(&controller);
    eDVSdevice.registerListener(&controller);
    terminput.registerListener(&controller);

    // start
    eDVSdevice.listen();
    rolls->listen();
    terminput.listen();
    controller.start();

    while(1) {
        usleep(1);
        if(flag){ // my action when signal set it 1
            std::cout << "abort signal caught!" << std::endl;
            break;
        } 
    }
		
    controller.stop();
    rolls->stopListening();
    eDVSdevice.stopListening();
    terminput.stopListening();
    eDVSdevice.close();

    if (enable_logging) {
	delete robot_logger;
	delete edvs_logger;
	delete rolls_logger;
	}

    usleep(1000);

    robot.setLeftVelocity(0);
    robot.setRightVelocity(0);

    return 0;
}

int main(int argc, char *argv[]) {
    // Variables that will store parsed values.
    std::string robot_address = "10.162.177.242";
    unsigned int robot_port = 56000;
    std::string udp_address = "127.0.0.1";
    std::string log_dir = "../logs";
    std::string exp_name = "";
    bool no_timestamp = false;

    bool setup_needed = false;
    bool reset_needed = false;
    double speed=1;
    double turn=1;

    // Command line args
    po::options_description desc("Pushbot Sequence Learning.\n\nOptions");
    desc.add_options()
    ("help", "produce help message")
    ("robot_address",
     po::value<std::string>(&robot_address)->default_value(robot_address),
     "ip address of robot")
    ("robot_port",
     po::value<unsigned int>(&robot_port)->default_value(robot_port),
     "port of robot")
    ("udp_address",
     po::value<std::string>(&udp_address)->default_value(udp_address),
     "ip address of log and display server")
    ("setup", po::bool_switch(&setup_needed),
     "set up rolls architecture")
    ("reset", po::bool_switch(&reset_needed),
	"reset rolls architecture")
    ("speed", po::value<double>(&speed)->default_value(speed),
     "factor to multiply speed")
    ("turn", po::value<double>(&turn)->default_value(turn),
     "factor to multiply turning speed")
    ("logdir", po::value<std::string>(&log_dir)->default_value(log_dir),
	"directory to store logs")
    ("exp", po::value<std::string>(&exp_name)->default_value(exp_name),
	"name of experiment (used for logfiles)")
    ("no-timestamp", po::bool_switch(&no_timestamp),
	"disable automatic adding of timestamp to experiment name")
    ;


    // Parse input
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    //std::cout << robot_address << robot_port << udp_address << setup_needed << reset_needed << std::endl;

    // Help message
    if (vm.count("help")) {std::cout << desc << "\n";return 1;}

    //Only enable logging if an experiment name is given
    bool enable_logging = !exp_name.empty();

    // Prepare Log directory and lof filenames
     if (log_dir.back() !='/') log_dir += '/';
     boost::filesystem::path dir(log_dir);     
     if(!(boost::filesystem::exists(dir))){         
	std::cout << "Creating directory '" << log_dir << "'..." << std::endl;;    
     		if (boost::filesystem::create_directory(dir))             	
			std::cout << "Done." << std::endl;     
	};     	
	if (!no_timestamp)         
		exp_name += "_" + to_iso_string(pt::second_clock::local_time());     
		std::string edvs_log = log_dir + exp_name + "_edvs.csv";     
		std::string imu_log = log_dir + exp_name + "_imu.csv";     
		std::string rolls_log = log_dir + exp_name + "_rolls.csv";     
		std::string robot_log = log_dir + exp_name + "_robot.csv";


    std::cout << "Connecting to robot over TCP on " << robot_address
              << ":" << robot_port << "..." << std::endl;

//     Robot and eDVS file descriptor
     int file_descriptor = getTCPFileDescriptor(
        robot_address.c_str(), robot_port );

  //   register signal handler for program abort
     signal(SIGINT, aborter); 

    return loop(file_descriptor, file_descriptor, setup_needed, reset_needed,
		enable_logging, edvs_log, rolls_log, robot_log, imu_log);
}
