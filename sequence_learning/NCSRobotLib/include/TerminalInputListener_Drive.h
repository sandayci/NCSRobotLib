#ifndef TERMINALINPUT_LISTENER_DRIVE_H
#define TERMINALINPUT_LISTENER_DRIVE_H

#include "TerminalInputListener.h"
#include "Enums.h"
#include "ROLLSDevice.h"
#include "ROLLSArchitecture.h"
//#include "SequenceLearning.h"
#include "TerminalInputDevice.h"

/*! \file TerminalInputListener_Drive.h
 *  \brief This file provides all the functions with respect to the keyboard listener.
 */
class TerminalInputListener_Drive : public TerminalInputListener
{
public:
    TerminalInputListener_Drive();
    //void receivedNewTerminalInputEvent( char read_char);
    //robotDriveParameters*   driveParams;
    //void addRobot( Robot * );
    void addRolls ( ROLLSDevice *_rolls );
    bool isEnabled();
		//NeuronGroup o_1 = NeuronGroup(30, 49);
		//NeuronGroup o_2 = NeuronGroup(50, 69);
		//NeuronGroup o_3 = NeuronGroup(70, 89);
		//NeuronGroup rst = NeuronGroup(226,235);
		//NeuronGroup cos = NeuronGroup(236,255);

private :
    bool    enable;
    //int     keysPressed;
    //Robot * robot;
    //ROLLSDevice *rolls;
	  //NeuronGroup o_1 = NeuronGroup(30, 49);
	  //NeuronGroup o_2 = NeuronGroup(50, 69);
	  //NeuronGroup o_3 = NeuronGroup(70, 89);
	  //NeuronGroup rst = NeuronGroup(226,235);
		//NeuronGroup cos = NeuronGroup(236,255);

};

#endif
