#include <iostream>

#include <QUdpSocket>
#include <QDataStream>

#include <cmath>

#define PI 3.14159265359

int main(int argc, char* argv[])
{
    QUdpSocket socket;

    float rx,ry,rz,ra;
    float r = 1.0;
    for(int i=0; i<127;i++)
    {
         r -= 1./127;

         /** Try to send data **/
         float tx,ty,tz;
         tx = 0.5 + 0.5*r*cos(2*PI*i/50.);
         ty = 0.5 + 0.5*r*sin(2*PI*i/50.);
         tz = i/50.;
         float rx = 0., ry=1., rz =0., ra=PI*i/256.;
		 
        /** Example of how to send something to the visualizer **/
		QByteArray datagram;
		QDataStream out(&datagram, QIODevice::WriteOnly);
		out.setVersion(QDataStream::Qt_5_8);
        out << i << tx << ty << tz
			<< rx << ry << rz
			<< ra;
		socket.writeDatagram(datagram,QHostAddress::LocalHost,12345);
    }
	return 0;
}
