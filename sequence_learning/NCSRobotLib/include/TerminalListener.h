#ifndef Terminal_Listener_H
#define Terminal_LISTENER_H

/*! \class TerminalListener
 *  \brief Provides the necessary interface to Listen to keyboard events
 */
class TerminalListener
{
         public:
                        virtual void receivedNewTerminalInputEvent( char read_char ) = 0;
};

#endif

