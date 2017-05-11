#include <EBV_Visualizer.h>
#include <EBV_PosePlugin.h>

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication    application(argc,argv);
    PosePlugin      myPoseDrawerPlugin;

    VisualizerWindow window(&myPoseDrawerPlugin);
    window.show();

    return application.exec();
}
