#------------------------------------------------------------
#
#  Root CMake file for EBV_VISU
#
#  From the off-tree build directory, invoke:
#  $ cmake <PATH_TO_PROJECT_ROOT>
#
#  initial version: jmartel@ethz.ch
#
#------------------------------------------------------------

# ENSURE MINIMUM VERSION TO USE------------------------------
# In this version of the CMake architecture of the EBV_PROJECT
# the directives qt5_use_modules() are used, it requires CMake
# version 2.8.9 at least.
cmake_minimum_required(VERSION 2.8.9)
#------------------------------------------------------------

# ALLOWS MORE READABLE "IF THEN ELSE" CONSTRUCTS ------------
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
#------------------------------------------------------------

# NAME OF THE ROOT SOLUTION----------------------------------
project(EBV_VISU_PROJECT)
#------------------------------------------------------------

# BUILD TYPE-------------------------------------------------
set(CMAKE_BUILD_TYPE Debug)
#------------------------------------------------------------

# CURRENT VERSION NUMBER-------------------------------------
set(EBV_VISU_PROJECT_VERSION "0.0.1")

string(REGEX MATCHALL "[0-9]" EBV_VISU_PROJECT_VERSION_PARTS "${EBV_VISU_PROJECT_VERSION}")

list(GET EBV_VISU_PROJECT_VERSION_PARTS 0 EBV_VISU_PROJECT_VERSION_MAJOR)
list(GET EBV_VISU_PROJECT_VERSION_PARTS 1 EBV_VISU_PROJECT_VERSION_MINOR)
list(GET EBV_VISU_PROJECT_VERSION_PARTS 2 EBV_VISU_PROJECT_VERSION_PATCH)

set(EBV_VISU_PROJECT_VERSION_SOVERSION "${EBV_VISU_PROJECT_VERSION_MAJOR}.${EBV_VISU_PROJECT_VERSION_MINOR}")
set(EBV_VISU_PROJECT_VERSION_DLLVERSION "${EBV_VISU_PROJECT_VERSION_MAJOR}${EBV_VISU_PROJECT_VERSION_MINOR}")
#------------------------------------------------------------

# OPTIONS USER CAN CHOOSE------------------------------------
# -- The options to build the project
#None
# -- In case CMake cannot find the Qt5 install prefix ask the user to set it manually
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "/Users/yulia/Qt/5.8/clang_64")
#------------------------------------------------------------

# ADDITIONAL MACROS WE CREATED------------------------------
# -- To "load" the macros which can build new EBV_PROJECT modules
# and apps.
# -- To "load" the configuring files for the "uninstall-ebv" and
# "doc" target
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMakeModules")
include(${CMAKE_MODULE_PATH}/ebv_project_module.cmake)
include(${CMAKE_MODULE_PATH}/ebv_project_app.cmake)
#------------------------------------------------------------

# CONFIGURATION FOR THE INSTALL------------------------------
# -- Install directory
set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} CACHE FILEPATH "Installation directory (lib,include,bin)")
set(LIBRARY_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_BUILD_TYPE})
set(INCLUDE_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/include/)
set(EXECUTABLE_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/bin/${CMAKE_BUILD_TYPE})
# -- Install the rules in the right directory
set(DRIVER_RULES_INSTALL_PATH "/etc/udev/rules.d" CACHE FILEPATH "Installation directory (driver rules)")
file(GLOB RULE_FILES "${CMAKE_CURRENT_LIST_DIR}/Misc/*.rules")
install(FILES ${RULE_FILES} DESTINATION ${DRIVER_RULES_INSTALL_PATH})
#------------------------------------------------------------

# CONFIGURATION FOR THE UNINSTALL------------------------------
# -- Create a new uninstall-ebv target
configure_file( "${CMAKE_MODULE_PATH}/ebv_uninstall.in.cmake"
                "${CMAKE_CURRENT_BINARY_DIR}/ebv_uninstall.cmake"
                IMMEDIATE @ONLY)
add_custom_target(uninstall-ebv
                  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/ebv_uninstall.cmake)
#------------------------------------------------------------

# REQUIRED MODULES AND LIBRARIES-----------------------------
# -- Search the packages and external libraries to build our
# modules and apps
    message(STATUS "=== Searching for packages Qt5, OpenGL, Boost, Threads")
    # + Contrary to Qt4, Qt5 should be used with a FindPackage for
    # each component.
    # + We may also later require Qt5Network, Qt5Xml (ebv-gui)
    # & Qt5PrintSupport (ebv-thirdparty-qcustomplot)
    find_package(Qt5Widgets REQUIRED)
    find_package(Qt5OpenGL REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(Threads REQUIRED)

    if(Qt5Widgets_FOUND)
        message(STATUS "+ Found Qt5-Widgets")
    else()
        message(STATUS "- Not Found Qt5-Widgets")
    endif()

    if(Qt5OpenGL_FOUND)
        message(STATUS "+ Found Qt5-OpenGL")
    else()
        message(STATUS "- Not Found Qt5-OpenGL")
    endif()

    if(OPENGL_FOUND)
        message(STATUS "+ Found OpenGL")
    else()
        message(STATUS "- Not Found OpenGL")
    endif()

    if(Threads_FOUND)
         message(STATUS "+ Found pthread")
    else()
         message(STATUS "- Not found pthread")
    endif()

    if(Qt5Widgets_FOUND AND Qt5OpenGL_FOUND)
        message(STATUS "... Setting Qt5 configuration ...")
        set(CMAKE_AUTOMOC ON)
        set(CMAKE_INCLUDE_CURRENT_DIR ON)
    endif()

#------------------------------------------------------------

# CONFIGURATION FILE OPTIONS---------------------------------
# Example how to use an option (eg. With GPU based code using CUDA)
#
# Previously in this file the following definitions are written:
#
#       option(WITH_CUDA "Use the GPU .." ON)
#	if(WITH_CUDA)
#		find_package(CUDA)
#		...
#	endif(WITH_CUDA)
#				# - You should include a :
#				#      #ifdef WITH_CUDA
#				#          <the code to execute with gpu>
#				#      #endif
#				# in the code when you want to specify that
#                               # something has to be considered only when the
#                               # WITH_CUDA variable is set 'ON'
#		
#				# - When running CMAKE just add the 
#				#       -DWITH_GPU=ON
#				# option to set the WITH_CUDA variable 'ON'
#	
#	#VAR FOR THE CONFIGURATION FILE
#	set(PACKAGE "ebv_project")
#	set(PACKAGE_BUGREPORT "jmartel@ethz.ch")
#	set(PACKAGE_NAME "ebv_lib")
#	set(PACKAGE_STRING "${PACKAGE} ${EBV_PROJECT_VERSION}")
#	set(PACKAGE_TARNAME "${PACKAGE}")
#	set(PACKAGE_VERSION "${EBV_PROJECT_VERSION}")
#	
#ADD THE OPTIONS IN A ebv_project_config.h.cmake FILE
configure_file(${CMAKE_MODULE_PATH}/EBV_ProjectConfig.h.cmake ${CMAKE_MODULE_PATH}/CMakeInclude/EBV_ProjectConfig.h)
include_directories(${CMAKE_MODULE_PATH}/CMakeInclude)
#------------------------------------------------------------

# COMPILATION OPTIONS----------------------------------------
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}") #-Wall
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ")

include(CheckCXXCompilerFlag)
if(CMAKE_COMPILER_IS_GNUCXX)
    check_cxx_compiler_flag("-Ofast" HAS_OFAST)
    if(HAS_OFAST)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast")
    endif()
endif()
#------------------------------------------------------------

# LAUNCH CMAKE IN SUBDIRECTORIES-----------------------------
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

# BUILD ALL THE THIRD PARTY LIBS/APPS
add_subdirectory(ThirdParty)

# BUILD ALL THE MODULES
add_subdirectory(Modules)

# BUILD ALL THE APPLICATIONS
add_subdirectory(Applications)
#------------------------------------------------------------
