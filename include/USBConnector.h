/*! \file USBConnector.h
 * \brief This file provides all the necessary functions to open a serial port to a USB connected device.
 * 
 * This is possible because the camera an robot are fitted with an FTDI chip.
 */

#ifndef USB_CONNECTOR_H
#define USB_CONNECTOR_H

#include <linux/serial.h>
#include <termio.h>


/*! \fn static speed_t getBaudrate( int baudrate );
 * \brief convert a numeric baudrate to a speed_t that can be used to open the serial port
 * \param baudrate the numeric value of the requested baudrate.
 * \return speed_t the necessary speed_t struct to open the serial port with the requested baudrate.
 */
static speed_t getBaudrate( int baudrate );

//! Opens and configures the serial port
/*! The requested device is opened with read and write enabled. The requested baudrate is set and the serial port 
 * configured to be in raw mode, non-blocking and with hardware control enabled. For more information see also here: 
 * \sa http://inilabs.com/support/hardware/edvs/#h.2v48aal08cim
 * \param port_name the name of the port is device is connected to, as listed under 'ls /dev/'
 * \param arg_baudrate the requested communication baudrate
 * \returns if successful return a valid file descriptr to the port, else -1.
 */
int getUSBFileDescriptor(char const * port_name ="/dev/ttyUSB0", unsigned int arg_baudrate=2000000, std::string devName="Device" )
{
    int m_ttyFd;
    std::cout<<"> Initializing USB connection ... "<<std::endl;
    std::cout<<"Trying to open on port: "<<port_name<<std::endl;
    int* baudrate = new int();
    *baudrate = arg_baudrate;

    struct termios tty;
    struct serial_struct ser;
    /* set the requested baudrate */
    if (!baudrate) 
        return -1; /* a problem occurred while allocating baudrate, return */
    
    // open the device
    if ((m_ttyFd = open(port_name, O_RDWR)) < 0)
    {
	std::cout<<"Could not find/open USB device"<<std::endl;
        return 0;
    }
    std::cout<<"Found/opened USB device"<<std::endl;
    
    // try to set a standard baudrate
    speed_t baudrateFlags = getBaudrate(*baudrate);
    
    // else try to set your own baudrate
    if (baudrateFlags == 0) 
    {
        baudrateFlags = B38400;
        if (ioctl(m_ttyFd, TIOCGSERIAL, &ser) < 0)
            return 0;
        ser.flags &= ~ASYNC_SPD_MASK;
        ser.flags |= ASYNC_SPD_CUST;
        ser.custom_divisor = ser.baud_base / *baudrate;
        if (ioctl(m_ttyFd, TIOCSSERIAL, &ser) < 0)
            return 0;
        if (ioctl(m_ttyFd, TIOCGSERIAL, &ser) < 0)
            return 0;
        // check if it worked
        if (*baudrate != (ser.baud_base / ser.custom_divisor))
        {
            *baudrate = ser.baud_base / ser.custom_divisor;
	    std::cout<<"Failed to set requested baudrate."<<std::endl;
            return 0;
        }
    }
    std::cout << "Baudrate: " << *baudrate << std::endl;
    tcgetattr(m_ttyFd, &tty); /* get record of parameters */
    cfsetospeed(&tty, baudrateFlags); /* set output speed */
    cfsetispeed(&tty, baudrateFlags); /* set input speed */
    speed_t obaudrate = cfgetispeed(&tty);
    std::cout << "Baudrate flags set: " << obaudrate << std::endl;

    /*
     * The serial port is configured as non canonical mode, ie, raw mode.
     */
    // configure input processing (i.e. raw / non-canonical)
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    // configure output processing
    tty.c_oflag &= ~OPOST;
    // configure local processing
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    /* control flags (bitmask) */
    tty.c_cflag &= ~(CSIZE | PARENB); // no parity
    tty.c_cflag |= CS8; // 8 bits
    tty.c_cflag |= CLOCAL | CREAD; // CLOCAL: ignore CD (carrier Detect) signal; CREAD: enable receiver
    // enable harware flow control / handshaking. [requires _BSD_SOURCE or _SVID_SOURCE]
    tty.c_cflag |= CRTSCTS;
    // minum number of charcters read before read returns
    tty.c_cc[VMIN] = 0;
    // 1 second timeout
    tty.c_cc[VTIME] = 10;
    /* configure port immediatly (TCSCANOW) */
    if (tcsetattr(m_ttyFd, TCSANOW, &tty))
    {
	std::cout<<"SerPort fatal: error during configuration!"<<std::endl;
        return 0;
    }
    //Set the serial port to blocking mode
    if (fcntl(m_ttyFd, F_SETFL, 0))
    {
        ::close(m_ttyFd);
        return -1;
    }

    std::cout<<"Initialization Successful!"<<std::endl;
    return m_ttyFd;
}

static speed_t getBaudrate( int baudrate )
{
    switch (baudrate) 
    {
        case 50:
           return B50;
        case 75:
           return B75;
        case 110:
           return B110;
        case 134:
           return B134;
        case 150:
           return B150;
        case 200:
           return B200;
        case 300:
           return B300;
        case 600:
           return B600;
        case 1200:
           return B1200;
        case 1800:
           return B1800;
        case 2400:
           return B2400;
        case 4800:
           return B4800;
        case 9600:
           return B9600;
        case 19200:
           return B19200;
        case 38400:
           return B38400;
        case 57600:
           return B57600;
        case 115200:
           return B115200;
        case 230400:
           return B230400;
        case 460800:
           return B460800;
        case 500000:
           return B500000;
        case 576000:
           return B576000;
        case 921600:
           return B921600;
        case 1000000:
           return B1000000;
        case 1152000:
           return B1152000;
        case 1500000:
           return B1500000;
        case 2000000:
           return B2000000;
        case 2500000:
           return B2500000;
        case 3000000:
           return B3000000;
        case 3500000:
           return B3500000;
        case 4000000:
           return B4000000;
    }
    return 0;
}

#endif
