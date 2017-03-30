#ifndef SEQUENCE_LEARNING
#define SEQUENCE_LEARNING

// Connections to ROLLS, eDVS, Pushbot
#include <ROLLSArchitecture.h>
#include <ROLLSDevice.h>
#include <ROLLSListener.h>
#include <EDVSListener.h>
#include <EDVSListener_UDP.h>
#include <EDVSDevice.h>
#include <PushBot.h>
#include <TerminalInputDevice.h>
#include <Enums.h>
#include <PlasticSynapses.h>

// Needed for firing rate timer
#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>

class SequenceLearning : public EDVSListener,
                                    public ROLLSArchitecture,
                  		    public ROLLSListener,
		        	    public TerminalInputListener,
				    public PlasticSynapses
{
																													
    public:
        SequenceLearning();
	SequenceLearning(ROLLSDevice *_rolls);
	SequenceLearning(ROLLSDevice *_rolls,
                         PushBot *_pushbot,
			 EDVSDevice *_eDVSdevice);
	
        // Listen to eDVS events
        void receivedNewEDVSEvent(EDVSEvent& event);
        void receivedNewEDVSIMUEvent(EDVSIMUEvent& event);
        // Listen to ROLLS events
        void receivedNewROLLSEvent(unsigned int event);
	// Keyboard Listener	
	void receivedNewTerminalInputEvent(char read_char);

        // Start/Stop the driving population
        void start(void);
        void stop(void);
	bool isEnabled();
	
    private:
        // ROLLS and pushbot
        ROLLSDevice *rolls;
        PushBot *pushbot;
	EDVSDevice *eDVSdevice;
				
	//keys
	bool enable;

	TerminalInput terminput;  
        // Neuron Groups used in rolls
        NeuronGroup mem_1;
        NeuronGroup mem_2;
        NeuronGroup mem_3;
        NeuronGroup ord_1;
        NeuronGroup ord_2;
        NeuronGroup ord_3;
	NeuronGroup content_DNF;
	NeuronGroup inh_DNF;
      	NeuronGroup reset;
      	NeuronGroup CoS;
	NeuronGroup all;
        // Helper to set up architecture
        void create_architecture();
};

#endif
