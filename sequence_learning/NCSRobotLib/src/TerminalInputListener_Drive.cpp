#include <iostream>
#include "TerminalInputListener_Drive.h"
//#include <SequenceLearning.h>
#include <stdio.h>
//#include <ROLLSDevice.h>
//#include <TerminalInputDevice.h>

TerminalInputListener_Drive::TerminalInputListener_Drive()
    :   enable(false)
        //keysPressed(0)
{
}

void TerminalInputListener_Drive::addRolls( ROLLSDevice *_rolls )
{
   // ROLLSDevice *rolls;
}

/*void TerminalInputListener_Drive::receivedNewTerminalInputEvent( char read_char )
{
	  std::cout << "Start TerminalInputListener Drive" << std::endl;
   
		//std::cout << "Start Keys" << std::endl;  
		switch(read_char)
	    {
	  case 'g' : // Go signal
			std::cout << "Start Keys" << std::endl;

			rolls->stimulate(o_1);
	      std::cout << "Go signal received" << std::endl;
				break;
		case 'c' : // CoS signal
        rolls->stimulate(cos);
				std::cout << "CoS signal received" << std::endl;;
				break;
		case 'r' : // reset signal
				rolls->stimulate(rst);
		    std::cout << "reset signal received" << std::endl;
				break;
		case 'h': //2nd ordinal group
				rolls->stimulate(o_2);
				std::cout << "second ordinal group stimulated" << std::endl;
				break;
		case 'j':  //3rd ordinal group
				rolls->stimulate(o_3);
				std::cout << "third ordinal group stimulated" << std::endl;
			  break;
				}
}
*/
