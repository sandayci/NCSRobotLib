#------------------------------------------------------------
#
#  TestTracker application CMake file for EBV_PROJECT
#
#  initial version: jmartel@ethz.ch
#
#------------------------------------------------------------

#LAUNCH MACRO TO BUILD THE APPLICATION-----------------------
# First argument in the macro is the application name,
# Second and other arguments are the dependancies

define_ebv_project_app(testDataSender
                       Qt5Network
                       )
#------------------------------------------------------------
