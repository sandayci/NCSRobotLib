#ifndef ROLLS_ARCHITECTURE_DRAWER_H
#define ROLLS_ARCHITECTURE_DRAWER_H


#include <ROLLSArchitecture.h>
#include <opencv2/opencv.hpp>

class ROLLSArchitecture_Drawer 
{
	public:
	        ROLLSArchitecture_Drawer(ROLLSArchitecture *_rolls_arch) : rolls_arch(_rolls_arch)
		{
			window = "ROLLS Architecture visualised";
			
			// N_NEURONS is defined by ROLLSArchitecture.h, included by the arch
			img.create(N_NEURONS, N_NEURONS, CV_8UC3);	
		} ;
	       void drawArchitecture();
	private:	
	        ROLLSArchitecture *rolls_arch;
	        std::string window;
		cv::Mat img;		
		int setting;
		int index;
		unsigned char r, g, b;
};

#endif
