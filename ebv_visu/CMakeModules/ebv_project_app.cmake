#------------------------------------------------------------
#
#  Macro file to build a new example for the EBV_PROJECT
#  - The example can depend on other libraries, these dependancies
#  are then specified in the arguments of the macro.
#
#  initial version: jmartel@ethz.ch
#
#------------------------------------------------------------

macro(define_ebv_project_app name)
     # CONFIGURATION OF THE PROJECT--------------------------
		# NAME OF THE PROJECT
                project(ebv-${name})
		
		# TARGET CONFIGURATION
                set(THE_TARGET_TEST "ebv-${name}")

                # ADD THE HEADERS OF THE APPLICATION BEING CREATED TO THE INCLUDE PATH
                include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
		
		# ADD THE INTERNAL AND 3rd PARTY LIBRARIES TO INCLUDE-PATH, LINK-PATH 
		# AND DEFINITIONS
		foreach(d ${ARGN})
                        if(${d} MATCHES "Qt5")
                            string(REPLACE "Qt5" "" QT_MODULE ${d})
                            list(APPEND QT_MODULES ${QT_MODULE})
                        endif()

                        if(${d} MATCHES "boost")
                                include_directories(${Boost_INCLUDE_DIRS})
                                list(APPEND 3RD_PARTY_LIBS ${Boost_LIBRARIES})
                        endif()

                        if(${d} MATCHES "opengl")
                                include_directories(${OPENGL_INCLUDE_DIR})
                                link_directories(${OPENGL_LIB_DIR})
                                list(APPEND 3RD_PARTY_LIBS ${OPENGL_LIBRARIES})
                        endif()

                        if(${d} MATCHES "opencv")
                                include_directories(${OPENCV_INCLUDE_DIR})
                                link_directories(${OPENCV_LIB_DIR})
                                list(APPEND 3RD_PARTY_LIBS ${OpenCV_LIBS})
                        endif()

                        if(${d} MATCHES "openni2")
                                include_directories(${OPENNI2_INCLUDE_DIR})
                                link_directories(${OPENNI2_LIBS_DIR})
                                list(APPEND 3RD_PARTY_LIBS ${OPENNI2_LIBRARIES})
                        endif()

                        if(${d} MATCHES "pthread")
                                list(APPEND 3RD_PARTY_LIBS ${CMAKE_THREAD_LIBS_INIT})
                        endif()

                        if(${d} MATCHES "libusb")
                                include_directories(${LIBUSB_INCLUDE_DIRS})
                                list(APPEND 3RD_PARTY_LIBS ${LIBUSB_LIBRARIES})
                        endif()

                        if(${d} MATCHES "gurobi")
                                include_directories(${GUROBI_INCLUDE_DIRS})
                                list(APPEND 3RD_PARTY_LIBS ${GUROBI_LIBRARIES})
                        endif()
			
			if(${d} MATCHES "ebv-")
                                string(REPLACE "ebv-" "${CMAKE_CURRENT_SOURCE_DIR}/../../Modules/" EBV_PROJECT_MODULE ${d})
                                include_directories(${EBV_PROJECT_MODULE}/include)
                                list(APPEND EBV_PROJECT_LIBS ${d})
			endif()

                        if(${d} MATCHES "ebv-thirdparty-")
                                string(REPLACE "ebv-thirdparty-" "${CMAKE_CURRENT_SOURCE_DIR}/../../ThirdParty/" EBV_PROJECT_MODULE ${d})
                                include_directories(${EBV_PROJECT_MODULE}/include)
                                list(APPEND EBV_PROJECT_LIBS ${d})
                        endif()
		endforeach()
        #----------------------------------------------------
	
        # CONFIGURATION OF THE EXECUTABLE--------------------
		# WHAT ARE THE FILES TO BUILD THE TARGET
		file(GLOB_RECURSE THE_TARGET_TEST_CXX ${CMAKE_CURRENT_SOURCE_DIR}/src/*)
		file(GLOB_RECURSE THE_TARGET_TEST_HDR ${CMAKE_CURRENT_SOURCE_DIR}/include/*)
                file(GLOB_RECURSE THE_TARGET_TEST_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/lib/*)

		# ADD THEM TO BUILD THE EXECUTABLE
		add_executable(	${THE_TARGET_TEST}
			      	${THE_TARGET_TEST_CXX}
				${THE_TARGET_TEST_HDR}
                                ${THE_TARGET_TEST_LIBS}
			       )
		
		target_link_libraries(	${THE_TARGET_TEST}
                                        ${THE_TARGET_TEST_LIBS}
				        ${EBV_PROJECT_LIBS}
					${3RD_PARTY_LIBS}
                                      )

                list(LENGTH QT_MODULES NUM_QT_MODULES)
                if(${NUM_QT_MODULES} GREATER 0)
                    qt5_use_modules(${THE_TARGET_TEST} ${QT_MODULES})
                endif()

                if(ARGN)
                        add_dependencies(${THE_TARGET_TEST} ${ARGN})
                endif()
        #----------------------------------------------------
        
        # INSTALL THE EXECUTABLES IN THE BIN DIR-------------
                install(TARGETS ${THE_TARGET_TEST}
			DESTINATION ${EXECUTABLE_INSTALL_PATH}
	           	)
        #----------------------------------------------------
endmacro()
