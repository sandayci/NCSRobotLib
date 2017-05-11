#------------------------------------------------------------
#
#  Macro file to build a new module for the EBV_PROJECT
#  - The module can depend on other modules, these dependancies
#  are then specified in the arguments of the macro.
#
#  initial version: jmartel@ethz.ch
#
#------------------------------------------------------------

macro(define_ebv_project_module name)
     # CONFIGURATION OF THE PROJECT--------------------------
		# NAME OF THE PROJECT
                project(ebv-${name})
		
		# TARGET CONFIGURATION
                set(THE_TARGET "ebv-${name}")
		
		# ADD THE HEADERS OF THE LIBRARY BEING CREATED TO THE INCLUDE PATH
		include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
		
		# ADD THE INTERNAL AND 3rd PARTY LIBRARIES TO INCLUDE-PATH, LINK-PATH 
		# AND DEFINITIONS
                set(CUDA_LIB FALSE)
		foreach(d ${ARGN})
                        if(${d} MATCHES "Qt5")
                            string(REPLACE "Qt5" "" QT_MODULE ${d})
                            list(APPEND QT_MODULES ${QT_MODULE})
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

                        if(${d} MATCHES "boost")
                                include_directories(${Boost_INCLUDE_DIRS})
                                list(APPEND 3RD_PARTY_LIBS ${Boost_LIBRARIES})
                        endif()

                        if(${d} MATCHES "cuda")
                            include_directories(${CUDA_INCLUDE_DIRS})
                            list(APPEND 3RD_PARTY_LIBS ${CUDA_LIBRARIES})

                            set(CUDA_LIB TRUE)
                            include_directories(${CMAKE_CURRENT_SOURCE_DIR}/cu)
                            set(CUDA_NVCC_FLAGS "-arch=compute_30")
                        endif()

                        if(${d} MATCHES "pthread")
                                list(APPEND 3RD_PARTY_LIBS ${CMAKE_THREAD_LIBS_INIT})
                        endif()

                        if(${d} MATCHES "libusb")
                                include_directories(${LIBUSB_INCLUDE_DIRS})
                                list(APPEND 3RD_PARTY_LIBS ${LIBUSB_LIBRARIES})
                        endif()

                        if(${d} MATCHES "arrayfire")
                                include_directories(${ArrayFire_INCLUDE_DIRS})
                                list(APPEND 3RD_PARTY_LIBS ${ArrayFire_LIBRARIES})
                        endif()

                        if(${d} MATCHES "gurobi")
                                include_directories(${GUROBI_INCLUDE_DIRS})
                                list(APPEND 3RD_PARTY_LIBS ${GUROBI_LIBRARIES})
                        endif()
			
			if(${d} MATCHES "ebv-")
                            message(STATUS ${d})
				string(REPLACE "ebv-" "${CMAKE_CURRENT_SOURCE_DIR}/../" EBV_PROJECT_MODULE ${d})
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
	
        # CONFIGURATION OF THE LIBRARY-----------------------
		# WHAT ARE THE FILES TO BUILD THE TARGET
		file(GLOB_RECURSE THE_TARGET_CXX ${CMAKE_CURRENT_SOURCE_DIR}/src/*)
		file(GLOB_RECURSE THE_TARGET_HDR ${CMAKE_CURRENT_SOURCE_DIR}/include/*)

		# ADD THEM TO BUILD THE LIBRARY
                if(CUDA_LIB)
                    file(GLOB_RECURSE THE_TARGET_CU ${CMAKE_CURRENT_SOURCE_DIR}/cu/*)
                    cuda_add_library(   ${THE_TARGET}
                                        ${THE_TARGET_CU}
                                        ${THE_TARGET_CXX}
                                        ${THE_TARGET_HDR}
                                    )
                else()
                    add_library(    ${THE_TARGET}
                                    ${THE_TARGET_CXX}
                                    ${THE_TARGET_HDR}
                                )
                endif()

		target_link_libraries(	${THE_TARGET}
					${EBV_PROJECT_LIBS}
					${3RD_PARTY_LIBS}		
				      )

                list(LENGTH QT_MODULES NUM_QT_MODULES)
                if(${NUM_QT_MODULES} GREATER 0)
                    qt5_use_modules(${THE_TARGET} ${QT_MODULES})
                endif()

		if(ARGN)
			add_dependencies(${THE_TARGET} ${ARGN})
		endif()
        #----------------------------------------------------
	
        # INSTALL THE HEADERS/LIBS IN THE INCLUDE/LIB DIR----
		install(FILES ${THE_TARGET_HDR} 
			DESTINATION ${INCLUDE_INSTALL_PATH} 
		       )
                install(TARGETS ${THE_TARGET}
			DESTINATION ${LIBRARY_INSTALL_PATH}
	            	)
        #----------------------------------------------------
        
endmacro()
