#ifndef EBV_VISUALIZER_H
#define EBV_VISUALIZER_H

#include <QMainWindow>
#include <QGLWidget>
#include <QPoint>
#include <QHostAddress>
#include <QVector3D>
#include <QQuaternion>
#include <QTimer>

QT_BEGIN_NAMESPACE
    class QWidget;
    class QTimer;
    class QUdpSocket;
    class QGroupBox;
    class QCheckBox;
    class QLineEdit;
    class QMenu;
    class QAction;
    class QBoxLayout;
    class QSpacerItem;
    class QSlider;
QT_END_NAMESPACE

class VisualizerGL;
class VisualizerWindow;

/**
 *
 * Convenient utility functions for graphics and images
 *
**/
void hsv_to_rgb(float h, float s, float v,
                unsigned char* r, unsigned char* g, unsigned char* b);
void lookAt(GLdouble eyex, GLdouble eyey, GLdouble eyez,
            GLdouble centerx, GLdouble centery, GLdouble centerz,
            GLdouble upx, GLdouble upy, GLdouble upz );
void setPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);

/**
 *
 * When you want to create your plugin for a visualizer window
 *
**/
class VisualizerPlugin : public QWidget
{
Q_OBJECT

public:
    VisualizerPlugin() {}
    ~VisualizerPlugin() {}

    virtual void createUserInterface(QGroupBox* group) = 0;
    virtual void decode(QByteArray& d) = 0;
    virtual void draw() = 0;
    virtual void drawHover(float objX, float objY, float objZ,
                           float zoom) = 0;

    void setWindow(VisualizerWindow* window)     {m_window = window;}
    void setVisualizer(VisualizerGL* visualizer) {m_visualizer = visualizer;}

protected:
    VisualizerWindow*   m_window;
    VisualizerGL*       m_visualizer;

};

/**
 *
 * The window, where the opengl window will be, the menus, the dock,
 * also what created the UDP server
 *
**/
class VisualizerWindow : public QMainWindow
{
    Q_OBJECT

public:
    VisualizerWindow(VisualizerPlugin* plugin);
    ~VisualizerWindow();

private:
    void setEventsDecoderDrawer(VisualizerPlugin* decoder);

    void createServer(QHostAddress addr = QHostAddress::LocalHost,
                      quint16 port = 12345);
    void createUserGroup();
    void createViewGroup();
    void createConnectionGroup();
    void createMenus();
    void createDock();

private slots:
    void readPendingDatagrams();
    void changeAddressServer();
    void changeDockLayout(Qt::DockWidgetArea dir);

private:
    // To implement your own drawing and decoding callbacks
    VisualizerPlugin*    m_decoderDrawer;
    QUdpSocket*          m_socket;

    // Central widgets
    QWidget*        m_centralWidget;
    VisualizerGL*   m_visualizer;

    // Dock
    QDockWidget*    m_dock;
    QBoxLayout*     m_dockLayout;
    QSpacerItem*    m_dockSpacer;

    // Views
    QGroupBox*      m_viewGroup;
    QGroupBox*      m_userGroup;
    QGroupBox*      m_connectionGroup;

    // Components
    QCheckBox*      m_floorCheck;
    QCheckBox*      m_skyboxCheck;
    QLineEdit*      m_connectEdit;
    QSlider*        m_transSensitivitySlider;
    QSlider*        m_rotSensitivitySlider;

    // Menus & action
    QMenu*          m_fileMenu;
    QMenu*          m_viewMenu;
    QMenu*          m_helpMenu;
    QAction*        m_closeAct;
    QAction*        m_aboutAct;
    QAction*        m_controlsAct;
};

/**
 *
 * The opengl widget in which everything happens for drawing
 *
**/
class VisualizerGL : public QGLWidget
{
    Q_OBJECT

public:
    VisualizerGL(QWidget* parent = 0);
    ~VisualizerGL();

    void setEventsDecoderDrawer(VisualizerPlugin* decoder);

signals:
    void xRotationChanged();
    void yRotationChanged();
    void zRotationChanged();
    void xTranslationChanged();
    void yTranslationChanged();
    void zTranslationChanged();

public slots:
    void toggleViewSkybox(bool t);
    void toggleViewFloor(bool t);
    void setCameraRotSensitivity(int s);
    void setCameraTransSensitivity(int s);

public:
    void setCamera();

    void drawAxis(float tx, float ty, float tz,
                  float angle, float rx, float ry, float rz,
                  float sx, float sy, float sz);
    void drawCamera(float tx, float ty, float tz,
                    float angle, float rx, float ry, float rz,
                    float sx, float sy, float sz);
    void drawFloor();
    void drawSkybox();
    void drawText(float x, float y, float z,
                  const QString &str, const QFont &fnt);
    void drawText(int x, int y,
                  const QString& str, const QFont& fnt = QFont());

private:
    void makeFloorTexture(int transparency);
    void makeFloor();
    void makeCamera();
    void makeCubeMap();
    void makeCubeMapTexture();
    void makeAxis();

private slots:
    void updateView();
    void updateViewDir();
    void keyMoveEvent();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

private:
    // To implement your own drawing and decoding callbacks
    VisualizerPlugin*     m_decoderDrawer;

    // Call lists
    GLuint                   m_axis;
    GLuint                   m_camera;
    GLuint                   m_skybox;
    GLuint                   m_plane;
    // Texture ids
    GLuint                   m_floor;
    GLuint                   m_cubeMap[6];
    // Hold the textures
    std::vector<QImage*>     m_cubeMapTex;
    GLubyte                  m_floorTex[16];

    bool                     m_drawSky;
    bool                     m_drawFloor;

    // Position of the camera and model
    // -- pos
    QVector3D   m_pos;
    QVector3D   m_viewDir;
    QVector3D   m_rightDir;
    QVector3D   m_upDir;
    float       m_horAngle;
    float       m_vertAngle;

    float m_zoom;
    float m_ar;

    QPoint m_lastPos;
    QPoint m_lastPos2;

    bool   m_ctrlPressed;

    // motion
    QTimer keyTimer;
    bool highspeed_move;
    bool key_move_left;
    bool key_move_right;
    bool key_move_forward;
    bool key_move_backward;
    bool key_move_up;
    bool key_move_down;
    bool key_rot_right;
    bool key_rot_left;
    bool key_rot_up;
    bool key_rot_down;
    bool key_rot_clockwise;
    bool key_rot_anticlockwise;
    float m_cameraRotSensitivity;
    float m_cameraTransSensitivity;

    // FPS
    QTimer* m_timer;
};

#endif // EBV_VISUALIZER_H
