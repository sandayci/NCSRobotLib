#------------------------------------------------------------
#
#  Visualizer module CMake file for EBV_OMNI_PROJECT
#
#  initial version: jmartel@ethz.ch
#
#------------------------------------------------------------

#LAUNCH MACRO TO BUILD THE APPLICATION-----------------------
# First argument in the macro is the application name,
# Second and other arguments are the dependancies

    define_ebv_project_module(visualizer
                              Qt5Widgets
                              Qt5OpenGL
                              Qt5Network
                              opengl
                              opencv
                              )
#------------------------------------------------------------
