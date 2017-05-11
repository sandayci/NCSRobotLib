#ifndef EBV_POSEPLUGIN_H
#define EBV_POSEPLUGIN_H

#include <EBV_Visualizer.h>

QT_BEGIN_NAMESPACE
    class QPushButton;
    class QCheckBox;
QT_END_NAMESPACE

class PosePlugin : public VisualizerPlugin
{
Q_OBJECT

public:
    PosePlugin();
    ~PosePlugin();

    virtual void createUserInterface(QGroupBox* userGroup);

    virtual void decode(QByteArray& datagram);

    virtual void draw();

    virtual void drawHover(float objX, float objY, float objZ,
                           float zoom); //TODO better give you the whole projection


public slots:
    void addToPath(int n,float x, float y, float z,
                   float rx, float ry, float rz, float a);
    void clearPath();

private:
    std::vector<QVector3D>   m_poseT;
    std::vector<QQuaternion> m_poseR;
    std::vector<int>         m_poseN;

    QPushButton*             m_clearButton;
    QCheckBox*               m_edgesCheck;
};

#endif // EBV_POSEPLUGIN_H
