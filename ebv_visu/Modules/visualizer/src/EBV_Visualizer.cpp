#include <EBV_Visualizer.h>

#include <iostream>

#include <QApplication>

#include <QWidget>
#include <QDockWidget>
#include <QMenuBar>
#include <QMessageBox>

#include <QGridLayout>
#include <QBoxLayout>

#include <QMouseEvent>
#include <QWheelEvent>
#include <QTimer>

#include <QVector3D>
#include <QQuaternion>

#include <QUdpSocket>
#include <QDataStream>

#include <QImage>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>

#include <QStringList>

#ifdef _WIN32
    #include <gl/GLU.h>
#elif __linux__ 
    #include <GL/glu.h>
#elif __APPLE__
  #include <OpenGL/gl.h>
   #include <OpenGL/glu.h>
   #include <OpenGL/glext.h>
   #include <GLUT/glut.h>
#endif

#include <cmath>
#include <math.h>

VisualizerWindow::VisualizerWindow(VisualizerPlugin* plugin)
    : QMainWindow(),
      m_socket(NULL)
{
    /** Visualizer **/
    m_visualizer = new VisualizerGL();

    /** Plugin **/
    setEventsDecoderDrawer(plugin);

    /** Server **/
    createServer();

    /** Interface groups **/
    createConnectionGroup();
    createViewGroup();
    createUserGroup();

    /** Dock **/
    createDock();

    /** Menus and Actions **/
    createMenus();

    /** Central widget **/
    setCentralWidget(m_visualizer);

    setWindowTitle("SLIM - Visualizer");
    setMinimumSize(700,480);
}

VisualizerWindow::~VisualizerWindow()
{
    if(m_visualizer != NULL)
        delete m_visualizer;
}

void VisualizerWindow::setEventsDecoderDrawer(VisualizerPlugin* decoder)
{
    m_decoderDrawer = decoder;
    m_decoderDrawer->setWindow(this);

    if(m_visualizer)
        m_visualizer->setEventsDecoderDrawer(m_decoderDrawer);
}

void VisualizerWindow::createDock()
{
    /** Control dock widget **/
    m_dock = new QDockWidget("Controls");
    QWidget* widget = new QWidget();
    m_dockLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    m_dockLayout->setDirection(QBoxLayout::TopToBottom);
    m_dockLayout->addWidget(m_connectionGroup);//  ,0,0, 1,1);
    m_dockLayout->addWidget(m_viewGroup);      //  ,1,0, 1,1);
    m_dockLayout->addWidget(m_userGroup);      //  ,2,0, 1,1);
    m_dockSpacer = new QSpacerItem(1,100,
                                    QSizePolicy::Minimum,
                                    QSizePolicy::Expanding);
    m_dockLayout->addSpacerItem(m_dockSpacer);
    widget->setLayout(m_dockLayout);
    m_dock->setWidget(widget);

    connect(m_dock,SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
            this,  SLOT(changeDockLayout(Qt::DockWidgetArea)));

    connect(m_dock,&QDockWidget::visibilityChanged,
            this, [=](bool vis)
                    {if(vis) m_controlsAct->setChecked(true);
                     else m_controlsAct->setChecked(false);
                    }
                    );

    this->addDockWidget(Qt::RightDockWidgetArea,m_dock);
}

void VisualizerWindow::createMenus()
{
    /** Actions **/
    m_closeAct = new QAction(tr("&Close"), this);
    m_closeAct->setStatusTip(tr("Close the application..."));
    connect(m_closeAct, SIGNAL(triggered(bool)),
            this, SLOT(close()));

    m_controlsAct = new QAction(tr("&Controls"), this);
    m_controlsAct->setStatusTip(tr("Show/Hide controls..."));
    m_controlsAct->setCheckable(true);
    m_controlsAct->setChecked(true);
    connect(m_controlsAct,&QAction::triggered,
            m_dock, [=](bool){if(m_dock->isVisible()) m_dock->hide();
                               else m_dock->show();});

    m_aboutAct = new QAction(tr("&About"), this);
    m_aboutAct->setStatusTip(tr("About this application..."));
    connect(m_aboutAct, &QAction::triggered,
            this, [=](bool)
                  {QMessageBox::about(this, tr("About Menu"),
                                      tr("The <b>SLIM Visualizer</b> is a small 3D visualizer for your "
                                         "data: send it to UDP and it plots it. "
                                         "<b>jmartel@ini.ethz.ch - 2017</b>"));
                  }
            );

    /** Menus **/
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_closeAct);

    m_viewMenu = menuBar()->addMenu(tr("&View"));
    m_viewMenu->addAction(m_controlsAct);

    m_helpMenu = menuBar()->addMenu(tr("&Help"));
    m_helpMenu->addAction(m_aboutAct);
}

void VisualizerWindow::createServer(QHostAddress addr, quint16 port)
{
    if(m_socket != NULL)
        delete m_socket;

    m_socket = new QUdpSocket(this);
    m_socket->bind(addr, port);

    connect(m_socket, SIGNAL(readyRead()),
            this, SLOT(readPendingDatagrams()));
}

void VisualizerWindow::createConnectionGroup()
{
    m_connectionGroup = new QGroupBox("Connection");

    QGridLayout* layout = new QGridLayout(m_connectionGroup);

    QLabel* label = new QLabel("addr:port");
    m_connectEdit = new QLineEdit();
    m_connectEdit->setText(QString("127.0.0.1:12345"));
    m_connectEdit->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

    connect(m_connectEdit,&QLineEdit::textEdited,
            this,[=](QString)
                   {m_connectEdit->setStyleSheet("QLineEdit { background: rgb(233, 99, 0); selection-background-color: rgb(233, 99, 0); }");
                   }
            );
    connect(m_connectEdit,SIGNAL(returnPressed()),
            this,SLOT(changeAddressServer()));

    layout->addWidget(label         ,0,0, 1,1);
    layout->addWidget(m_connectEdit ,1,0, 1,1);
}

void VisualizerWindow::createUserGroup()
{
    m_userGroup = new QGroupBox("User defined");

    if(m_decoderDrawer != NULL)
        m_decoderDrawer->createUserInterface(m_userGroup);
}

void VisualizerWindow::createViewGroup()
{
    m_viewGroup = new QGroupBox("View");

    QGridLayout* layout = new QGridLayout(m_viewGroup);

    // Create checkboxes
    m_floorCheck = new QCheckBox();
    m_floorCheck->setText("View floor");
    m_floorCheck->setChecked(true);
    connect(m_floorCheck,SIGNAL(toggled(bool)),
            m_visualizer,SLOT(toggleViewFloor(bool)));

    m_skyboxCheck = new QCheckBox();
    m_skyboxCheck->setText("View sky");
    m_skyboxCheck->setChecked(true);
    connect(m_skyboxCheck,SIGNAL(toggled(bool)),
            m_visualizer,SLOT(toggleViewSkybox(bool)));

    QLabel* labelTrans = new QLabel("Trans: 50");
    m_transSensitivitySlider = new QSlider(Qt::Horizontal);
    m_transSensitivitySlider->setSingleStep(1);
    m_transSensitivitySlider->setRange(1,100);
    connect(m_transSensitivitySlider,SIGNAL(valueChanged(int)),
            m_visualizer,SLOT(setCameraTransSensitivity(int)));
    connect(m_transSensitivitySlider,&QSlider::valueChanged,
            this,[=](int s) {labelTrans->setText("Trans: "+QString::number(s,'g',3));});
    m_transSensitivitySlider->setValue(50);

    QLabel* labelRot = new QLabel("Rot: 50");
    m_rotSensitivitySlider = new QSlider(Qt::Horizontal);
    m_rotSensitivitySlider->setSingleStep(1);
    m_rotSensitivitySlider->setRange(1,100);
    connect(m_rotSensitivitySlider,SIGNAL(valueChanged(int)),
            m_visualizer,SLOT(setCameraRotSensitivity(int)));
    connect(m_rotSensitivitySlider,&QSlider::valueChanged,
            this,[=](int s) {labelRot->setText("Rot: "+QString::number(s,'g',3));});
    m_rotSensitivitySlider->setValue(50);

    layout->addWidget(m_floorCheck              ,0,0, 1,2);
    layout->addWidget(m_skyboxCheck             ,1,0, 1,2);
    layout->addWidget(labelTrans                ,2,0 ,1,1);
    layout->addWidget(m_transSensitivitySlider  ,2,1, 1,1);
    layout->addWidget(labelRot                  ,3,0 ,1,1);
    layout->addWidget(m_rotSensitivitySlider    ,3,1, 1,1);
}

void VisualizerWindow::changeAddressServer()
{
    QString s = m_connectEdit->text();

    QStringList sList = s.split(':');
    /*
    std::cout << "Parts: " << std::endl;
    for(int i=0; i<sList.size(); i++)
        std::cout << sList.at(i).toStdString() << std::endl;
    */

    if(sList.size() != 2)
    {
        std::cout << "Not a valid address" << std::endl;
        return;
    }

    // Extract address
    QHostAddress addr;
    bool isAddr = addr.setAddress(sList.at(0));
    if(!isAddr)
    {
        std::cout << "Not a valid address" << std::endl;
        return;
    }

    // Extract port
    quint16 port = (quint16)sList.at(1).toInt();

    //std::cout << "Split:" << addr.toString().toStdString() << " " << port << std::endl;
    m_connectEdit->setStyleSheet("QLineEdit { background: rgb(255, 255, 255); selection-background-color: rgb(255, 255, 255); }");
    createServer(addr,port);
}

void VisualizerWindow::changeDockLayout(Qt::DockWidgetArea dir)
{
    if(dir == Qt::RightDockWidgetArea ||
       dir == Qt::LeftDockWidgetArea)
    {
        m_dockLayout->removeItem(m_dockSpacer);
        if(m_dockSpacer != NULL)
            delete m_dockSpacer;
        m_dockSpacer = new QSpacerItem(1,1,
                                       QSizePolicy::Minimum,
                                       QSizePolicy::Expanding);
        m_dockLayout->addSpacerItem(m_dockSpacer);
        m_dockLayout->setDirection(QBoxLayout::TopToBottom);

    }
    else
    {
        m_dockLayout->removeItem(m_dockSpacer);
        if(m_dockSpacer != NULL)
            delete m_dockSpacer;
        m_dockSpacer = new QSpacerItem(1,1,
                                       QSizePolicy::Expanding,
                                       QSizePolicy::Minimum);
        m_dockLayout->addSpacerItem(m_dockSpacer);
        m_dockLayout->setDirection(QBoxLayout::LeftToRight);
    }
}

void VisualizerWindow::readPendingDatagrams()
{
    QByteArray datagram;
    while(m_socket->hasPendingDatagrams())
    {
        //std::cout << "Received a data point" << std::endl;
        datagram.resize(m_socket->pendingDatagramSize());
        m_socket->readDatagram(datagram.data(),datagram.size());

        if(m_decoderDrawer != NULL)
            m_decoderDrawer->decode(datagram);
    }
}






VisualizerGL::VisualizerGL(QWidget *parent)
    : QGLWidget(parent),
      m_decoderDrawer(0),
      key_move_left(false),
      key_move_right(false),
      key_move_forward(false),
      key_move_backward(false),
      key_move_up(false),
      key_move_down(false),
      key_rot_right(false),
      key_rot_left(false),
      key_rot_up(false),
      key_rot_down(false),
      key_rot_clockwise(false),
      key_rot_anticlockwise(false),
      m_cameraTransSensitivity(0.01),
      m_cameraRotSensitivity(0.001)
{
    // Initialize the zoom
    m_zoom = 1.0;

    // Refresh the view
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateView()));
    m_timer->start(20);

    m_drawFloor = true;
    m_drawSky = true;

    // Key modifier
    m_ctrlPressed = false;

    // To capture key events
    setFocusPolicy(Qt::StrongFocus);

      m_axis=0;
      m_camera=0;
      m_skybox=0;
      m_plane=0;
      m_floor=0;
      for(int i =0; i<6; i++)
        m_cubeMap[i] = 0;

    // New stuff
    m_pos = QVector3D(3.15249, 1.21098, 3.12621);
    m_horAngle = -2.3425;
    m_vertAngle = -0.16;
    updateViewDir();
    keyTimer.setInterval(5);
    connect(&keyTimer,SIGNAL(timeout()),this,SLOT(keyMoveEvent()));

    makeCurrent();
    initializeGL();
}

VisualizerGL::~VisualizerGL()
{
    makeCurrent();

    // Delete the call lists
    glDeleteLists(m_axis, 1);
    glDeleteLists(m_camera, 1);
    glDeleteLists(m_skybox, 6);
    glDeleteLists(m_plane,1);
    // Textures
    glDeleteTextures(1, &m_floor);
    glDeleteTextures(6, m_cubeMap);
}

void VisualizerGL::setEventsDecoderDrawer(VisualizerPlugin *decoder)
{
    m_decoderDrawer = decoder;
    m_decoderDrawer->setVisualizer(this);
}

void VisualizerGL::updateView()
{
    updateGL();
}

void VisualizerGL::initializeGL()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH, GL_NICEST);

    glEnable(GL_NORMALIZE);

    glClearColor(0.0f,0.0f,0.0f,1.0f);

    makeAxis();
    makeCamera();
    makeFloorTexture(64);
    makeFloor();
    makeCubeMapTexture();
    makeCubeMap();
}

void VisualizerGL::setCamera()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
        setPerspective(45.0*m_zoom,m_ar,1e-3,1e4);
        //glOrtho(-2*m_ar*m_zoom,2*m_ar*m_zoom,-2*m_zoom,2*m_zoom,0,1e5);
}


void VisualizerGL::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();

        QVector3D posE = m_pos + m_viewDir;
        lookAt(m_pos.x(),m_pos.y(),m_pos.z(),
                  (m_pos + m_viewDir).x(),(m_pos + m_viewDir).y(),(m_pos + m_viewDir).z(),
                  m_upDir.x(),m_upDir.y(),m_upDir.z());

        /** DRAW THE SKYBOX **/
        if(m_drawSky)
            drawSkybox();

        /** DRAW THE FLOOR**/
        if(m_drawFloor)
            drawFloor();

        /** DRAW THE AXIS **/
        drawAxis(0.,0.,0.,
                 0.,0.,0,1.0,
                 1.,1.,1.);

        /** DRAW USER DEFINED STUFF **/
        if(m_decoderDrawer != NULL)
            m_decoderDrawer->draw();

        /** DRAW THE SELECTED POINT FRAME **/
        QPoint p = this->mapFromGlobal(QCursor::pos());

        GLdouble model[16],proj[16];
        GLint view[4];

        glGetDoublev(GL_MODELVIEW_MATRIX, model);
        glGetDoublev(GL_PROJECTION_MATRIX, proj);
        glGetIntegerv(GL_VIEWPORT, view);

        GLdouble winX, winY;
        GLfloat winZ;
        winX = p.x();
        winY = view[3] - p.y();

        glReadPixels(GLint(winX), GLint(winY),
                     1,1, GL_DEPTH_COMPONENT,
                     GL_FLOAT, &winZ);

        GLdouble objX, objY, objZ;
        gluUnProject(winX, winY, winZ,
                     model, proj, view,
                     &objX, &objY, &objZ);

        /** DRAW USER DEFINED STUFF WHEN HOVERING ON AN OBJECT**/
        if(m_decoderDrawer != NULL)
            m_decoderDrawer->drawHover(objX,objY,objZ,m_zoom);
    glPopMatrix();
}

void VisualizerGL::resizeGL(int width, int height)
{
    int side = qMin(width, height);

    glViewport(0,0,width,height);

    double w = width;
    double h = height;
    m_ar = w / h;

    setCamera();
}

void VisualizerGL::drawCamera(float tx, float ty, float tz,
                              float angle, float rx, float ry, float rz,
                              float sx, float sy, float sz)
{
    glPushMatrix();
        glTranslatef(tx, ty, tz);
        glRotatef(angle, rx, ry, rz);
        glScalef(sx, sy, sz);

        glCallList(m_camera);
    glPopMatrix();
}

void VisualizerGL::drawAxis(float tx, float ty, float tz,
                            float angle, float rx, float ry, float rz,
                            float sx, float sy, float sz)
{
    glPushMatrix();
        glTranslatef(tx, ty, tz);
        glRotatef(angle, rx, ry, rz);

        glScalef(sx, sy, sz);

        glCallList(m_axis);
    glPopMatrix();
}

void VisualizerGL::drawFloor()
{
    glPushMatrix();
        glCallList(m_plane);
    glPopMatrix();
}

void VisualizerGL::drawSkybox()
{
    glPushMatrix();
        glRotatef(-90.0,1,0,0);
        glCallList(m_skybox);
    glPopMatrix();
}

void VisualizerGL::drawText(float x, float y, float z,
                            const QString &str, const QFont &fnt)
{
    renderText(x,y,z,str,fnt);
}

void VisualizerGL::drawText(int x, int y,
                            const QString &str, const QFont &fnt)
{
    renderText(x,y,str,fnt);
}

void VisualizerGL::makeCubeMapTexture()
{
    // For the textures and faces of the cubemaps
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(6, m_cubeMap);

    /** Load images **/
    std::vector<std::string> textures;
    std::string files("TropicalSunnyDay");
    textures.push_back("../../../Misc/skybox/"+files+"/"+files+"Left2048.png");  // left
    textures.push_back("../../../Misc/skybox/"+files+"/"+files+"Up2048.png");    // up
    textures.push_back("../../../Misc/skybox/"+files+"/"+files+"Front2048.png"); // front
    textures.push_back("../../../Misc/skybox/"+files+"/"+files+"Down2048.png");  // down
    textures.push_back("../../../Misc/skybox/"+files+"/"+files+"Right2048.png"); // right
    textures.push_back("../../../Misc/skybox/"+files+"/"+files+"Back2048.png");  // back

    for(int i=0; i<textures.size(); i++)
    {
        // Load image
        QImage* image = new QImage();
        image->load(textures[i].c_str());
        m_cubeMapTex.push_back(image);

        GLsizei sw = image->width();
        GLsizei sh = image->height();

        //Textures
        glBindTexture(GL_TEXTURE_2D, m_cubeMap[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
            gluBuild2DMipmaps( GL_TEXTURE_2D, 3, sw, sh, GL_BGRA, GL_UNSIGNED_BYTE, image->bits());
            /*
            glTexImage2D(GL_TEXTURE_2D, 0 , GL_BGRA,
                         sw, sh,
                         0, GL_BGRA, GL_UNSIGNED_BYTE,
                         image->bits());
                         */
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void VisualizerGL::makeCubeMap()
{
    /** Generate Call List **/
    m_skybox = glGenLists(1);
    glNewList(m_skybox, GL_COMPILE);
        const float f = 100.0;
        const float c = 0.5*f;

        const float aa=1, bb=0, // 1 0
                    cc=0, dd=0, // 0 0
                    ee=0, ff=1, // 0 1
                    gg=1, hh=1; // 1 1
        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glBindTexture(GL_TEXTURE_2D, m_cubeMap[0]); // LEFT
        glBegin(GL_QUADS);
            glTexCoord2f(aa, bb); glVertex3f(-c, -c, +c);
            glTexCoord2f(cc, dd); glVertex3f(-c, +c, +c);
            glTexCoord2f(ee, ff); glVertex3f(-c, +c, -c);
            glTexCoord2f(gg, hh); glVertex3f(-c, -c, -c);
        glEnd();
        glBindTexture(GL_TEXTURE_2D,0);

        glBindTexture(GL_TEXTURE_2D, m_cubeMap[1]); // TOP
        glBegin(GL_QUADS);
            glTexCoord2f(aa, bb); glVertex3f(-c, -c, +c);
            glTexCoord2f(cc, dd); glVertex3f(+c, -c, +c);
            glTexCoord2f(ee, ff); glVertex3f(+c, +c, +c);
            glTexCoord2f(gg, hh); glVertex3f(-c, +c, +c);
        glEnd();
        glBindTexture(GL_TEXTURE_2D,0);

        glBindTexture(GL_TEXTURE_2D, m_cubeMap[2]); // FRONT
        glBegin(GL_QUADS);
            glTexCoord2f(aa, bb); glVertex3f(-c, +c, +c);
            glTexCoord2f(cc, dd); glVertex3f(+c, +c, +c);
            glTexCoord2f(ee, ff); glVertex3f(+c, +c, -c);
            glTexCoord2f(gg, hh); glVertex3f(-c, +c, -c);
        glEnd();
        glBindTexture(GL_TEXTURE_2D,0);

        glBindTexture(GL_TEXTURE_2D, m_cubeMap[3]); // BOTTOM
        glBegin(GL_QUADS);
            glTexCoord2f(aa, bb); glVertex3f(-c, +c, -c);
            glTexCoord2f(cc, dd); glVertex3f(+c, +c, -c);
            glTexCoord2f(ee, ff); glVertex3f(+c, -c, -c);
            glTexCoord2f(gg, hh); glVertex3f(-c, -c, -c);
        glEnd();
        glBindTexture(GL_TEXTURE_2D,0);

        glBindTexture(GL_TEXTURE_2D, m_cubeMap[4]); // RIGHT
        glBegin(GL_QUADS);
            glTexCoord2f(aa, bb); glVertex3f(+c, +c, +c);
            glTexCoord2f(cc, dd); glVertex3f(+c, -c, +c);
            glTexCoord2f(ee, ff); glVertex3f(+c, -c, -c);
            glTexCoord2f(gg, hh); glVertex3f(+c, +c, -c);
        glEnd();
        glBindTexture(GL_TEXTURE_2D,0);

        glBindTexture(GL_TEXTURE_2D, m_cubeMap[5]); // BACK
        glBegin(GL_QUADS);
            glTexCoord2f(aa, bb); glVertex3f(+c, -c, +c);
            glTexCoord2f(cc, dd); glVertex3f(-c, -c, +c);
            glTexCoord2f(ee, ff); glVertex3f(-c, -c, -c);
            glTexCoord2f(gg, hh); glVertex3f(+c, -c, -c);
        glEnd();
        glBindTexture(GL_TEXTURE_2D,0);
    glEndList();
}

void VisualizerGL::makeFloorTexture(int transparency)
{
    m_floorTex[0] = 64;    m_floorTex[1] = 64;    m_floorTex[2] = 64;     m_floorTex[3] = transparency;
    m_floorTex[4] = 128;   m_floorTex[5] = 128;   m_floorTex[6] = 128;    m_floorTex[7] = transparency;
    m_floorTex[8] = 128;   m_floorTex[9] = 128;   m_floorTex[10] = 128;   m_floorTex[11] = transparency;
    m_floorTex[12] = 64;   m_floorTex[13] = 64;   m_floorTex[14] = 64;    m_floorTex[15] = transparency;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    if(m_floor != 0)
        glDeleteTextures(1, &m_floor);
    glGenTextures(1, &m_floor);
    glBindTexture(GL_TEXTURE_2D, m_floor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 2, 2,
                 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 m_floorTex);

    glBindTexture(GL_TEXTURE_2D,0);
}

void VisualizerGL::makeFloor()
{
    m_plane = glGenLists(1);
    glNewList(m_plane, GL_COMPILE);
        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D, m_floor);

        glBegin(GL_QUADS);
            glColor3f(.5,.5,.5);
            // Draws an infinite plane (w=0 allows to place a point at infinity)
            glTexCoord2f(0.0, 0.0);     glVertex3f(-100,-0.1,-100);
            glTexCoord2f(0.0, 500.0);   glVertex3f(-100,-0.1,100);
            glTexCoord2f(500.0, 500.0); glVertex3f(100,-0.1,100);
            glTexCoord2f(500.0, 0.0);   glVertex3f(100,-0.1,-100);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    glEndList();
}

void VisualizerGL::makeCamera()
{
    m_camera = glGenLists(1);
    glNewList(m_camera, GL_COMPILE);
        glLineWidth(5.0);
        glBegin(GL_LINES);
            glColor4f(1.,0.,0.5,1.0);
            glVertex3f(0.0,0.0,0.0);
            glVertex3f(1.0,0.0,0.0);

            glColor4f(0.,1.,0.5,1.0);
            glVertex3f(0.0,0.0,0.0);
            glVertex3f(0.0,1.0,0.0);

            glColor4f(0,1.,1.,1.0);
            glVertex3f(0.0,0.0,0.0);
            glVertex3f(0.0,0.0,1.0);
        glEnd();
        glLineWidth(1.0);
    glEndList();
}

void VisualizerGL::makeAxis()
{
    m_axis = glGenLists(1);
    glNewList(m_axis, GL_COMPILE);
        glLineWidth(5.0);
        glBegin(GL_LINES);
            glColor4f(1.0,0.0,0.0,1.0);
            glVertex3f(0.0,0.0,0.0);
            glVertex3f(1.0,0.0,0.0);

            glColor4f(0.0,1.0,0.0,1.0);
            glVertex3f(0.0,0.0,0.0);
            glVertex3f(0.0,1.0,0.0);

            glColor4f(0.0,0.0,1.0,1.0);
            glVertex3f(0.0,0.0,0.0);
            glVertex3f(0.0,0.0,1.0);
        glEnd();
        glLineWidth(1.0);
    glEndList();
}

void VisualizerGL::toggleViewSkybox(bool t)
{
    m_drawSky = t;
    if(t) makeFloorTexture(64);
    else  makeFloorTexture(255);
}

void VisualizerGL::toggleViewFloor(bool t)
{
    m_drawFloor = t;
}

void VisualizerGL::setCameraRotSensitivity(int s)
{
    m_cameraRotSensitivity = s/1e4;
}

void VisualizerGL::setCameraTransSensitivity(int s)
{
    m_cameraTransSensitivity = s/1e3;
}

void VisualizerGL::mousePressEvent(QMouseEvent *event)
{
    switch(event->buttons())
    {
        case Qt::LeftButton:
            m_lastPos = event->pos();
        break;

        case Qt::RightButton:
            m_lastPos2 = event->pos();
        break;
    }
}

void VisualizerGL::updateViewDir()
{
    m_viewDir.setX(cosf(m_vertAngle) * sinf(m_horAngle));
    m_viewDir.setY(sinf(m_vertAngle));
    m_viewDir.setZ(cosf(m_vertAngle) * cosf(m_horAngle));

    m_rightDir.setX(sinf(m_horAngle - 3.14f/2.0f));
    m_rightDir.setY(0);
    m_rightDir.setZ(cosf(m_horAngle - 3.14f/2.0f));

    m_upDir = QVector3D::crossProduct(m_rightDir, m_viewDir);
}

void VisualizerGL::mouseMoveEvent(QMouseEvent *event)
{
    float dx, dy;

    switch(event->buttons())
    {
        // Left button:
        // changes orientation (!ctrl) / position (ctrl)
        case Qt::LeftButton:
            dx = event->x() - m_lastPos.x();
            dy = event->y() - m_lastPos.y();

            if(!m_ctrlPressed)
            {
                m_horAngle  += 0.001*dx;
                m_vertAngle += 0.001*dy;
                updateViewDir();
            }
            else
            {
                m_pos += m_rightDir*0.01*dx - m_viewDir*0.01*dy;
            }

            m_lastPos = event->pos();
        break;

        // Right button:
        // changes position
        case Qt::RightButton:
            dx = event->x() - m_lastPos2.x();
            dy = event->y() - m_lastPos2.y();

            m_pos += m_rightDir*0.01*dx - m_viewDir*0.01*dy;

            m_lastPos2 = event->pos();
        break;
    }
}

void VisualizerGL::wheelEvent(QWheelEvent *event)
{
    float w = event->delta() / 120.0;

    m_zoom *= (1 + 0.1*w);

    // Clip high zoom
    if(m_zoom <= 1e-4)
        m_zoom = 1e-4;
    // Clip low zoom
    if(m_zoom > 150./45.)
        m_zoom = 150./45.;

    setCamera();
    updateGL();
}

void VisualizerGL::keyPressEvent(QKeyEvent* event)
{
    if(!event->isAutoRepeat())
    {
        //QWidget::keyPressEvent(event);

        bool shiftHeld = QApplication::keyboardModifiers() & Qt::ShiftModifier;
        highspeed_move = QApplication::keyboardModifiers() & Qt::AltModifier;

        switch(event->key()) {
            case Qt::Key_Control:
                m_ctrlPressed = true;
                break;
                /*
            default:
                event->ignore();
                break;*/

            case Qt::Key_A:
            case Qt::Key_Left:
                key_move_left = true;
                keyTimer.start();
                break;
            case Qt::Key_S:
            case Qt::Key_Down:
                key_move_down = shiftHeld;
                key_move_backward = !shiftHeld;
                keyTimer.start();
                break;
            case Qt::Key_D:
            case Qt::Key_Right:
                key_move_right = true;
                keyTimer.start();
                break;
            case Qt::Key_W:
            case Qt::Key_Up:
                key_move_up = shiftHeld;
                key_move_forward = !shiftHeld;
                keyTimer.start();
                break;
            case Qt::Key_Q:
                key_rot_right = !shiftHeld;
                key_rot_up = shiftHeld;
                keyTimer.start();
                break;
            case Qt::Key_E:
                key_rot_left = !shiftHeld;
                key_rot_down = shiftHeld;
                keyTimer.start();
                break;
            case Qt::Key_Z:
                key_rot_clockwise = true;
                keyTimer.start();
                break;
            case Qt::Key_C:
                key_rot_anticlockwise = true;
                keyTimer.start();
                break;
            default:
                break;
        }
    }
    event->accept();
}

void VisualizerGL::keyReleaseEvent(QKeyEvent* event)
{
    if(!event->isAutoRepeat()) {
        highspeed_move = QApplication::keyboardModifiers() & Qt::AltModifier;

        switch(event->key()) {
            case Qt::Key_Control:
                m_ctrlPressed = false;
                keyTimer.stop();
                break;
//            default:
//                event->ignore();
//                break;

            case Qt::Key_A:
            case Qt::Key_Left:
                key_move_left = false;
                keyTimer.stop();
                break;
            case Qt::Key_S:
            case Qt::Key_Down:
                key_move_down = false;
                key_move_backward = false;
                keyTimer.stop();
                break;
            case Qt::Key_D:
            case Qt::Key_Right:
                key_move_right = false;
                keyTimer.stop();
                break;
            case Qt::Key_W:
            case Qt::Key_Up:
                key_move_up = false;
                key_move_forward = false;
                keyTimer.stop();
                break;
            case Qt::Key_Q:
                key_rot_right = false;
                key_rot_up = false;
                keyTimer.stop();
                break;
            case Qt::Key_E:
                key_rot_left = false;
                key_rot_down = false;
                keyTimer.stop();
                break;
            case Qt::Key_Z:
                key_rot_clockwise = false;
                keyTimer.stop();
                break;
            case Qt::Key_C:
                key_rot_anticlockwise = false;
                keyTimer.stop();
                break;
            default:
                QWidget::keyReleaseEvent(event);
                break;
        }
    }
    event->accept();
}

void VisualizerGL::keyMoveEvent(void)
{
    const double speed = (highspeed_move)
                          ? m_cameraTransSensitivity*5.0 : m_cameraTransSensitivity*0.5;
    const double angle = (highspeed_move)
                          ? m_cameraRotSensitivity*5.0 : m_cameraRotSensitivity*2.5;

    if(key_rot_right) {
        m_horAngle += angle;
        updateViewDir();
    }
    if(key_rot_left) {
        m_horAngle += -angle;
        updateViewDir();
    }
    if(key_rot_up) {
        m_vertAngle += angle;
        updateViewDir();
    }
    if(key_rot_down) {
        m_vertAngle -= angle;
        updateViewDir();
    }
    if(key_rot_clockwise) {
        // TODO
    }
    if(key_rot_anticlockwise) {
        // TODO
    }
    if(key_move_forward) {
        m_pos += speed*m_viewDir;
    }
    if(key_move_backward) {
        m_pos += -speed*m_viewDir;
    }
    if(key_move_up) {
        m_pos += speed*m_upDir;
    }
    if(key_move_down) {
        m_pos += -speed*m_upDir;
    }
    if(key_move_left) {
        m_pos += -speed*m_rightDir;
    }
    if(key_move_right) {
        m_pos += speed*m_rightDir;
    }
}









void hsv_to_rgb(float h, float s, float v,
                unsigned char* r, unsigned char* g, unsigned char* b)
{
    if(s < 0) s = 0;
    if(s > 1) s = 1;
    if(v < 0) v = 0;
    if(v > 1) v = 1;

    if(s == 0) {
        *r = (unsigned char)(v*255);
        *g = (unsigned char)(v*255);
        *b = (unsigned char)(v*255);
    }

    h = h - floorf(h); // want h to be in 0..1

    unsigned int i = h*6;
    float f = (h*6) - i;
    float p = v*(1.0f - s);
    float q = v*(1.0f - s*f);
    float t = v*(1.0f - s*(1.0f-f));
    switch(i%6) {
    case 0:
        *r = (unsigned char)(v*255);
        *g = (unsigned char)(t*255);
        *b = (unsigned char)(p*255);
        return;
    case 1:
        *r = (unsigned char)(q*255);
        *g = (unsigned char)(v*255);
        *b = (unsigned char)(p*255);
        return;
    case 2:
        *r = (unsigned char)(p*255);
        *g = (unsigned char)(v*255);
        *b = (unsigned char)(t*255);
        return;
    case 3:
        *r = (unsigned char)(p*255);
        *g = (unsigned char)(q*255);
        *b = (unsigned char)(v*255);
        return;
    case 4:
        *r = (unsigned char)(t*255);
        *g = (unsigned char)(p*255);
        *b = (unsigned char)(v*255);
        return;
    case 5:
        *r = (unsigned char)(v*255);
        *g = (unsigned char)(p*255);
        *b = (unsigned char)(q*255);
        return;
    }
}

// This is a replacement for gluPerspective
void setPerspective(GLdouble  fovy,  GLdouble  aspect,  GLdouble  zNear,  GLdouble  zFar)
{
    GLdouble ymax = zNear * tan((fovy * 3.14159265359) / 360.0);
    glFrustum(-ymax*aspect, ymax*aspect, -ymax, ymax, zNear, zFar);
}

// This is a replacement for gluLookAt
void lookAt( GLdouble eyex, GLdouble eyey, GLdouble eyez,
             GLdouble centerx, GLdouble centery, GLdouble centerz,
             GLdouble upx, GLdouble upy, GLdouble upz )
{
   GLdouble m[16];
   GLdouble x[3], y[3], z[3];
   GLdouble mag;
   /* Make rotation matrix */
   /* Z vector */
   z[0] = eyex - centerx;
   z[1] = eyey - centery;
   z[2] = eyez - centerz;
   mag = sqrt( z[0]*z[0] + z[1]*z[1] + z[2]*z[2] );
   if (mag) {  /* mpichler, 19950515 */
      z[0] /= mag;
      z[1] /= mag;
      z[2] /= mag;
   }
   /* Y vector */
   y[0] = upx;
   y[1] = upy;
   y[2] = upz;
   /* X vector = Y cross Z */
   x[0] =  y[1]*z[2] - y[2]*z[1];
   x[1] = -y[0]*z[2] + y[2]*z[0];
   x[2] =  y[0]*z[1] - y[1]*z[0];
   /* Recompute Y = Z cross X */
   y[0] =  z[1]*x[2] - z[2]*x[1];
   y[1] = -z[0]*x[2] + z[2]*x[0];
   y[2] =  z[0]*x[1] - z[1]*x[0];
   /* cross product gives area of parallelogram, which is < 1.0 for
    * non-perpendicular unit-length vectors; so normalize x, y here
    */
   mag = sqrt( x[0]*x[0] + x[1]*x[1] + x[2]*x[2] );
   if (mag) {
      x[0] /= mag;
      x[1] /= mag;
      x[2] /= mag;
   }
   mag = sqrt( y[0]*y[0] + y[1]*y[1] + y[2]*y[2] );
   if (mag) {
      y[0] /= mag;
      y[1] /= mag;
      y[2] /= mag;
   }
#define M(row,col)  m[col*4+row]
   M(0,0) = x[0];  M(0,1) = x[1];  M(0,2) = x[2];  M(0,3) = 0.0;
   M(1,0) = y[0];  M(1,1) = y[1];  M(1,2) = y[2];  M(1,3) = 0.0;
   M(2,0) = z[0];  M(2,1) = z[1];  M(2,2) = z[2];  M(2,3) = 0.0;
   M(3,0) = 0.0;   M(3,1) = 0.0;   M(3,2) = 0.0;   M(3,3) = 1.0;
#undef M
   glMultMatrixd( m );
   /* Translate Eye to Origin */
   glTranslated( -eyex, -eyey, -eyez );
}
