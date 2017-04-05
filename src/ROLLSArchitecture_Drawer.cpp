#include "ROLLSArchitecture_Drawer.h"



void ROLLSArchitecture_Drawer::drawArchitecture()
{

    // Copied from DisplayEvents - is this necessary?
    #ifdef __APPLE__
       cv::namedWindow(window, 1);
    #else
       cv::namedWindow(window, 0);
    #endif

    // Draw nonplastic synapses
    for (int pre = 0; pre < N_NEURONS; pre++) 
    {
        for (int post = 0; post < N_NEURONS; post++) 
	{
            setting = rolls_arch->nonplastic[pre][post];

            switch (setting) {
                case 4: r = 180;
                        g = 30;
                        b = 85;
                        break;
                case 3: r = 255;
                        g = 65;
                        b = 65;
                        break;
                case 2: r = 255;
                        g = 90;
                        b = 140;
                        break;
                case 1: r = 255;
                        g = 180;
                        b = 220;
                        break;
                case -1: r = 140;
                         g = 215;
                         b = 255;
                        break;
                case -2: r = 60;
                         g = 150;
                         b = 255;
                         break;
                case -3: r = 0;
                         g = 50;
                         b = 255;
                         break;
                case -4: r = 25;
                         g = 25;
                         b = 150;
                         break;
                default: r = 255;
                         g = 255;
                         b = 255;
                         break;
            };

            // add to image, pre is horizontal axis, post is vertical axis
            // invert vertical axis (origin should be lower left corner)
            index = N_NEURONS * (N_NEURONS - 1 - pre) * 3 + post * 3;
            img.data[index + 0] = b;
            img.data[index + 1] = g;
            img.data[index + 2] = r;
        }
    }

    // Display
    cv::imshow(window, img);
    cv::waitKey();
}
