#include <EBV_PosePlugin.h>

#include <QByteArray>
#include <QDataStream>
#include <QPushButton>
#include <QGridLayout>
#include <QCheckBox>
#include <QGroupBox>

#include <cmath>

#ifdef __APPLE__
	#include <OpenGL/gl.h>
#elif
	#include <GL/gl.h>
#endif

PosePlugin::PosePlugin()
    : VisualizerPlugin()
{}

PosePlugin::~PosePlugin()
{}

void PosePlugin::createUserInterface(QGroupBox* userGroup)
{
    QGridLayout* layout = new QGridLayout(userGroup);

    // Create checkbox
    m_edgesCheck = new QCheckBox();
    m_edgesCheck->setText("View edges");
    m_edgesCheck->setChecked(true);

    // Create clear button
    m_clearButton = new QPushButton();
    m_clearButton->setText("Clear");
    connect(m_clearButton,SIGNAL(pressed()),
            this,SLOT(clearPath()));

    layout->addWidget(m_edgesCheck ,0,0, 1,1);
    layout->addWidget(m_clearButton,1,0, 1,1);
}

/** Sending things will look like: **/
/*
    QByteArray datagram;
    QDataStream out(&datagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_8);
    out << n << p.Tx << p.Ty << p.Tz
        << rx << ry << rz
        << ra;
    socket.writeDatagram(datagram,QHostAddress::LocalHost,12345);
*/
void PosePlugin::decode(QByteArray& datagram)
{
    QDataStream in(&datagram, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_8);
    int n;

    float tx, ty, tz, rx, ry, rz, a;
    in >> n >> tx >> ty >> tz
       >> rx >> ry >> rz >> a;

    this->addToPath(n,tx,ty,tz,
                      rx,ry,rz,a);
}

void PosePlugin::draw()
{
    glPushMatrix();
    if((m_poseT.size()>0) && (m_poseR.size()>0))
    {
        glPointSize(25.0);
        glBegin(GL_POINTS);
            int pathLength = m_poseT.size();
            for(int i=0; i<pathLength; i++)
            {
                unsigned char r,g,b;
                hsv_to_rgb(0.66-0.66*i/pathLength,1.,1.,&r,&g,&b);   // heatmap

                glColor4f(r/255.,g/255.,b/255.,1.);
                glVertex3f(m_poseT[i].x(), m_poseT[i].y(), m_poseT[i].z());
            }
        glEnd();

        if(m_edgesCheck->isChecked())
        {
            glBegin(GL_LINE_STRIP);
                pathLength = m_poseT.size();
                for(int i=0; i<pathLength; i++)
                {
                    unsigned char r,g,b;
                    hsv_to_rgb(0.66-0.66*i/pathLength,1.,1.,&r,&g,&b);   // heatmap

                    glColor4f(r/255.,g/255.,b/255.,1.);
                    glVertex3f(m_poseT[i].x(), m_poseT[i].y(), m_poseT[i].z());
                }
            glEnd();
        }

        float rx,ry,rz,ra;
        m_poseR.back().getAxisAndAngle(&rx,&ry,&rz,&ra);
        m_visualizer->
        drawCamera(m_poseT.back().x(),m_poseT.back().y(),m_poseT.back().z(),
                   ra,rx,ry,rz,
                   0.2,0.2,0.2);
    }
    glPopMatrix();
}

void PosePlugin::drawHover(float objX, float objY, float objZ,
                           float zoom)
{
    glPushMatrix();
    int minDistIndex = -1;
    int minDist = 1e9;
    float x,y,z;

    for(int i=0; i<m_poseT.size(); i++)
    {
        x = m_poseT[i].x();
        y = m_poseT[i].y();
        z = m_poseT[i].z();

        float d = sqrtf(
                            (objX-x)*(objX-x)
                          + (objY-y)*(objY-y)
                          + (objZ-z)*(objZ-z)
                   );

        if(d < 0.05*zoom)
        {
            if(d < minDist)
            {
                minDist = d;
                minDistIndex = i;
            }
        }
    }

    float rx, ry, rz, ra;
    float tx,ty,tz;
    int ind;
    if(minDistIndex >= 0)
    {

        tx = m_poseT[minDistIndex].x();
        ty = m_poseT[minDistIndex].y();
        tz = m_poseT[minDistIndex].z();
        ind = m_poseN[minDistIndex];
        m_poseR[minDistIndex].getAxisAndAngle(&rx,&ry,&rz,&ra);

        m_visualizer->
        drawCamera(tx,ty,tz,
                   ra,rx,ry,rz,
                   0.4*zoom,0.4*zoom,0.4*zoom);
    }

    /** DRAW THE PROJECTION ON THE XZ-plane and Y **/
    if(minDistIndex >= 0)
    {
        // X on XZ line
        glColor3f(0,0,1);
        glLineWidth(4.0);
        glBegin(GL_LINES);
            glVertex3f(tx,0,0);
            glVertex3f(tx,0,tz);
        glEnd();

        // Z on XZ line
        glColor3f(1,0,0);
        glLineWidth(4.0);
        glBegin(GL_LINES);
            glVertex3f(0,0,tz);
            glVertex3f(tx,0,tz);
        glEnd();

        // Y line
        glColor3f(0,1,0);
        glLineWidth(4.0);
        glBegin(GL_LINES);
           glVertex3f(tx,0,tz);
           glVertex3f(tx,ty,tz);
        glEnd();

        glColor3f(0.,0.,0);

        m_visualizer->
        drawText(5,25,"["
                           +QString::number(tx,'g',3)+","
                           +QString::number(ty,'g',3)+","
                           +QString::number(tz,'g',3)+
                         "]\n"
                  ,QFont("Arial",15,12));

        m_visualizer->
        drawText(5,55,QString::number(ra,'g',3)+"deg ["
                      +QString::number(rx,'g',3)+","
                      +QString::number(ry,'g',3)+","
                      +QString::number(rz,'g',3)+
                       "]"
                  ,QFont("Arial",15,12));
    }
    glPopMatrix();
}

void PosePlugin::addToPath(int n,
                           float x, float y, float z,
                           float rx, float ry, float rz,
                           float a)
{
    // Translation
    m_poseT.push_back(QVector3D(x,y,z));

    // Rotation
    m_poseR.push_back(QQuaternion::fromAxisAndAngle(rx,ry,rz,a*180/3.14));

    m_poseN.push_back(n);
}

void PosePlugin::clearPath()
{
    m_poseT.clear();
    m_poseR.clear();
    m_poseN.clear();
}
