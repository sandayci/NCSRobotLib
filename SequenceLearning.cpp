#include <SequenceLearning.h>
#include <stdlib.h>
#include <iostream>
#include <functional>
#include <bitset>
#include <aerctl.h>
#include <unistd.h>
#include "cJSON.h"
#include <fstream>
#include <fcntl.h>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <cassert>
#include <exception>
#include <string>
#include <PushBot.h>

using boost::property_tree::ptree;
using boost::property_tree::read_json;

#define DELAY 50  // look every __ spikes if time interval is done
#define INTERVAL 500 // time in milliseconds to check firing counts

// offset for content DNF, starts at neuron 90
unsigned int offset = 90;
unsigned int speed = 7;
// divide DVS x-axis pixels in three parts
unsigned int first_third = 42;
unsigned int second_third = 85;
unsigned int third_third = 128;
unsigned int half_field = 64;
// set recall to true so that robot turns towards active content neurons
bool recall = false;
// how long the robot needs to turn in order to face the learned laser pointer location
unsigned int time_turn_ms;
unsigned int time_diff;
boost::posix_time::ptime start_t;
boost::posix_time::ptime new_time;
boost::posix_time::time_duration diff;

 SequenceLearning::SequenceLearning(void)
    : rolls(0), pushbot(0) {
    // This constructor if you only want to inspect the architecture.
    create_architecture();
}

SequenceLearning::SequenceLearning(
				ROLLSDevice *_rolls)
				: rolls(_rolls) {
				
					all = NeuronGroup(0,255);
				}

SequenceLearning::SequenceLearning(
        ROLLSDevice *_rolls, PushBot *_pushbot, EDVSDevice *_eDVSdevice)
        : rolls(_rolls), pushbot(_pushbot), eDVSdevice(_eDVSdevice), enable(false) {

    		create_architecture();
}

void SequenceLearning::create_architecture(void) {

    // Connect Neuron Groups (the function apply() will put it on ROLLS)
    mem_1 = NeuronGroup(0, 14);
    mem_2 = NeuronGroup(15, 29);
    mem_3 = NeuronGroup(30, 44);
    ord_1 = NeuronGroup(45, 59);
    ord_2 = NeuronGroup(60, 74);
    ord_3 = NeuronGroup(75, 89);
    content_DNF = NeuronGroup(90,217);
    inh_DNF = NeuronGroup(218,225);
    reset = NeuronGroup(226,235);
    CoS = NeuronGroup(236,255);

    //connect all content neurons inhibitory to create the basis of the DNF inhibition	
    connectNonplastic(content_DNF, -2);
    // connectivity of DNF, excitation to the 3 nearest neighbors
    for (int i = 0; i < 128; i++ ) { 
      connectNonplastic(offset + i, offset + i, 2); 
      for (int j = 1; j < 4; j++) {
         if (offset + i + j >= offset + 128) {   
            connectNonplastic(offset + i, offset + i - j, 3); 
          }
         else if (offset + i -j < offset) {
            connectNonplastic(offset + i, offset + i + j, 3); 
         }    
         else {
           connectNonplastic(offset + i, offset + i -j, 3); 
           connectNonplastic(offset + i, offset + i + j, 3); 
         }
       }
    }   
	
    // connect memory and ordinal groups excitatory within each other
    connectNonplastic(mem_1, 2);   
    connectNonplastic(mem_2, 2);
    connectNonplastic(mem_3, 2);
    connectNonplastic(ord_1, 2);
    connectNonplastic(ord_2, 2);
    connectNonplastic(ord_3, 2);

    // Memory groups to ordinals
    connectNonplastic(mem_1, ord_2, 3);
    connectNonplastic(mem_2, ord_3, 3);
    connectNonplastic(mem_1, ord_1, -1);
    connectNonplastic(mem_2, ord_2, -1);
    connectNonplastic(mem_3, ord_3, -1);

    // Ordinals to memories and inhibitory towards each other
    connectNonplastic(ord_1, mem_1, 2);
    connectNonplastic(ord_2, mem_2, 2);
    connectNonplastic(ord_3, mem_3, 2);
    connectNonplastic(ord_1, ord_2, -3);
    connectNonplastic(ord_1, ord_3, -3);
    connectNonplastic(ord_2, ord_1, -3);
    connectNonplastic(ord_2, ord_3, -3);    
    connectNonplastic(ord_3, ord_1, -3);
    connectNonplastic(ord_3, ord_2, -3);

    // DNF connectivity, uncomment if you want to use inhibitory group
    //connectNonplastic(content_DNF, inh_DNF, 3);
    //connectNonplastic(inh_DNF, content_DNF, -2);

    // Reset group, not used at the moment
    connectNonplastic(reset, mem_1, -4);
    connectNonplastic(reset, mem_2, -4);
    connectNonplastic(reset, mem_3, -4);
    connectNonplastic(reset, ord_1, -4);
    connectNonplastic(reset, ord_2, -4);
    connectNonplastic(reset, ord_3, -4);

    // CoS inhibition, CoS group not used at the moment
    connectNonplastic(CoS, ord_1, -4);
    connectNonplastic(CoS, ord_2, -4);
    connectNonplastic(CoS, ord_3, -4);

    // Plastic: DNF with ordinals
    connectPlastic(ord_1, content_DNF, true);
    connectPlastic(ord_2, content_DNF, true);
    connectPlastic(ord_3, content_DNF, true);
}


void SequenceLearning::start(void) {
}

void SequenceLearning::stop(void) {
}

// Process  events from the eDVS
void SequenceLearning::receivedNewEDVSEvent(EDVSEvent& event) {
	// stimulate neurons of the DNF according to the x-axis poistion 
       // incoming DVS events
	for (int j = 0; j < 4; j++) {
		rolls->stimulate(offset + event.m_y, j);
	}
}

// Process IMU events from eDVS 
void SequenceLearning::receivedNewEDVSIMUEvent(EDVSIMUEvent& ev) {     
   
 }

// Process ROLLS events
void SequenceLearning::receivedNewROLLSEvent( unsigned int event) {
	// if robot should turn during recall, look at activity of content neurons to determine the turn
        if (recall == true && event > offset && event <= offset + 128) {
		// set recall false to execute the turn command only once
                recall = false;
		// is the activity on the left or on the right side of the visual field?
                // right, turn right
		if (event > offset + half_field) {
			  // how long robot has to turn depends on how far the activity is from the central line,
			  // dividing the visual field
                          time_turn_ms = (event-offset-half_field) * 50;
                          pushbot->setLeftVelocity(speed);
                          pushbot->setRightVelocity(0);
			  // stop turning to the right, then turn back to the left
                          usleep(time_turn_ms*1000);
                          pushbot->setLeftVelocity(0);
                          pushbot->setRightVelocity(speed);
                          usleep(time_turn_ms*1000);
                          pushbot->setLeftVelocity(0);
                          pushbot->setRightVelocity(0);
                          usleep(3000000);
                }
		// do the same if in the opposite direction if the acitivty is on the left
                else if (event < offset + half_field) {
                          time_turn_ms = (event-offset) * 50;
                          pushbot->setLeftVelocity(0);
                          pushbot->setRightVelocity(speed);
                          usleep(time_turn_ms*1000);
                          pushbot->setLeftVelocity(speed);
                          pushbot->setRightVelocity(0);
                          usleep(time_turn_ms*1000);
                          pushbot->setLeftVelocity(0);
                          pushbot->setRightVelocity(0);
                          usleep(3000000);
                 }
		// don't turn if acitivy is on the central line (middle)
                else if (event == offset+half_field) {
                  	pushbot->setLeftVelocity(0);
                  	pushbot->setRightVelocity(0);
                }
          }

}

void SequenceLearning::receivedNewTerminalInputEvent( char read_char ) {
	  std::cout << "Start TerminalInputListener Drive" << std::endl;
   
	 	int n = 133; //neuron to be monitored
		int s = 111; //synapse to be monitored

		//std::cout << "Start Keys" << std::endl;  
		switch(read_char)
	        {
			case 'g' : // Go signal, stimulate first ordinal to trigger the sequence
			std::cout << "Start Keys" << std::endl;
			rolls->stimulate(ord_1);
	      		std::cout << "Go signal received" << std::endl;
			break;
			
			case 'c' : // CoS signal
			// either stimulate CoS group
			// or inhbitory synapses of ordinal and content
			// used during recall
        		//rolls->stimulate(CoS);
			for (int i = 5; i < 7; i++) {
				rolls->stimulate(ord_1, i);
				rolls->stimulate(ord_2, i);
				rolls->stimulate(ord_3, i);
       				rolls->stimulate(content_DNF, i);
			}
			std::cout << "CoS signal received" << std::endl;;
			break;

			case 'r' : // reset signal
			//rolls->stimulate(reset);
			// stimulate either reset group
			// or inh synapses of ordinal, memory and content
		 	for (int i = 5; i < 7; i++) {
				rolls->stimulate(ord_1, i);
		   		rolls->stimulate(ord_2, i);
				rolls->stimulate(ord_3, i);
       				rolls->stimulate(mem_1, i);
       				rolls->stimulate(mem_2, i);
       				rolls->stimulate(mem_3, i);
         			rolls->stimulate(content_DNF, i);
         		}
			
			// stop listening to DVS, to do recall
			eDVSdevice->stopListening();
			// uncomment next line, if robot should turn
			// recall = true;
			std::cout << "reset signal received" << std::endl;
			break;

			case 'a':
			// listen to DVS whenver you like
         		eDVSdevice->listen();
       			break;
	
     			case 'b':
			// stop listening to DVS and disconnect all neurons
      			eDVSdevice->stopListening();
			for (int pre = 0; pre < 256; pre ++) {
				for (int post = 0; post < 256; post++) {
			        	rolls->setNonplasticSynapse(pre, post, false);
			//              rolls->setPlasticSynapse(pre, post, false);
			          }
			  }
			break;

			case 'j': // stop robots movement
			recall = false;
			pushbot->setLeftVelocity(0);
			pushbot->setRightVelocity(0);
			std::cout << "Stop moving" << std::endl;
			break;

			case 'k':  // allows robot to move
			recall = true;
			std::cout << "Start moving" << std::endl;
			break;
			
			case 'm':
			//switch on monitor for neuron n and synapse s
			// content layer 90 -190
			std::cout << "monitoring soma" <<std::endl;
			aerMonSomaSyn(n, s);
			break;

			case 'h': //set synapse high
			std::cout << "setting synapse high" << std::endl;
			aerSetSynHi(n, s);
			break;
		
			case 'l': //set synapse low
			aerSetSynLo(n, s);
			break;
	
			case 'o':
			// stimulate all plastic synapses of some neurons, as a test
          		for (int i = 160; i < 190; i++){
            			for (int j = 0; j < 255; j++) {
              				rolls->stimplastic(i,j);
          	         	 }
          		}
         		break;

			case 'n':
			// inhibit ordinal and memory groups
			for (int i = 5; i < 7; i++) {
				rolls->stimulate(ord_1, i);
		        	rolls->stimulate(ord_2, i);
		        	rolls->stimulate(ord_3, i);
		        	rolls->stimulate(mem_1, i);
		        	rolls->stimulate(mem_2, i);
		        	rolls->stimulate(mem_3, i);
				break;
			}
		
			case 'p':

/* this whole block would be for reading synapses:
   it switches off all listeners, disconnects neurons,
   sets readout biases manually through a given file 
			
			//char *build_path = BUILD_PATH;
		        rolls->stopListening();
		       //set wijget biases
		       // disconnct synapses     
			eDVSdevice->stopListening();
		       for (int pre = 0; pre < 256; pre ++) {
         		  for (int post = 0; post < 256; post++) {
             			rolls->setNonplasticSynapse(pre, post, false);
             			rolls->setPlasticSynapse(pre, post, false);
            		   }
         		}
 
       			std::cout << "synapses disconnected" << std::endl;
			 
        		boost::property_tree::ptree pt;
        		boost::property_tree::read_json("/home/parallella/code/sequence_new/src/sampler.json", pt);	  
       
      		       //set biases
                       ptree pt2 = pt.get_child("biasData");
                       for (auto& array_element : pt2) {
                       Bias bias = {.id = -1, .coarse = -1, .fine = -1, .high = -1, .type = -1, .cascode = -1, .enable = -1};
                       int a = 0;
                       for (auto& property : array_element.second) {
                         if (a == 0){
                           a++;  
                           continue;
                         } 
                         
                         else {
                           int value = property.second.get_value<int>();
 
                         //std::cout << "value: "<< value << std::endl;
 
                         if(a == 1) {
                             bias.id = value;
                           }
                           else if(a == 2){
                            bias.coarse = value;
                           }
                           else if(a == 3){
                            bias.fine = value;
                           }
                           else if(a == 4) {
                            bias.high = value;
                           }
                           else if(a == 5) {
                            bias.type = value;
                           }
                       
                           a++;
                          } 
                        }
                     	bias.cascode = 0;
                     	bias.enable = 1;
                     	sleep(1.0);
                     	aerSetBias(&bias);
     
        	             //  std::cout << "biasID: " << a << std::endl;
 	
                    }			 		    
*/
			
		    std::cout << "biases set" << std::endl;
		    // do the plastic synapse stimulation here, call PlasticSynapse.cpp,
		    // specify which neurons should be tested
		    rolls->listen();
		    readSynapses(*rolls, 45, 89);
		    break;
				}
}			
