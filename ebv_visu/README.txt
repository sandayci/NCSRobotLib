#########################################################################
#				README FILE				#
#									#
#	EBV-Interacting Visual Maps Project				#
#									#
#	Copyright: Julien MARTEL 2014 @ ETH-Zurich			#
#									#
#									#
#########################################################################

===== INSTALLATION ===== 

----- Requirements -----

- 1) You need CMake (a build system in its 2.8.9 version at least) to install
the project
- 2) Other libraries are needed depending on what you wish to install
OpenCV, OpenGL, OpenMP, OpenNI, Boost, LibUSB, Qt5
- 3) Doxygen is needed if you want to build the documentation

----- Instructions -----
On Linux (Tested on Ubuntu 12.04, 13.04, 13.10)

** (In the command line) **

- 1) In the ebv root directory create a build/ directory.
 
cd ebv
mkdir build

- 2) Use cmake or ccmake to configure the project in the build
directory (off-source build).
You can select different options in the GUI of cmake (ccmake)
depending on what you want to install, or by using the command line
calling "cmake -D<The option name>=ON" (to activate an option) and
"cmake -D<The option name>=OFF" to (deactivate an option).

cd build
ccmake ../

- 3) Build the project using make
You can also install it somewhere by changing the LIBRARY_INSTALL_PATH,
INCLUDE_INSTALL_PATH, EXECUTABLE_INSTALL_PATH in cmake and executing 
"make install" (by default, if these variables are not changed it will be
installed in "/usr/local/bin" "/usr/local/lib" "/usr/local/include".
You can also build the documentation (built in <ebv root folder>/Docs) by 
executing "make doc".

make 
(sudo make install)
(sudo make doc)

** (In QtCreator) **

- 1) Click on "Open File or Project" in the "File" menu

- 2) Select and open the CMakeList.txt in the ebv root directory

- 3) Run CMake 

- 4) Build the project


