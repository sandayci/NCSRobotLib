#include <SequenceLearning.h>
#include <opencv2/opencv.hpp>
//#include <boost/filesystem.hpp>

int main(int argc, char *argv[]) {
    // Create image and window
    std::string win = "Visualize Architecture";
    cv::Mat img; 
    cv::Mat img_p;
    cv::Mat img_gray;	

    // Copied from DisplayEvents - is this necessary?
    #ifdef __APPLE__
       cv::namedWindow(win, 1);
    #else
       cv::namedWindow(win, 0);
    #endif

    // N_NEURONS is defined by ROLLSArchitecture.h, included by the arch
    img.create(N_NEURONS, N_NEURONS, CV_8UC3);
    img_p.create(N_NEURONS, N_NEURONS, CV_8UC3);    

    // Load Architecture
    SequenceLearning controller;

    // Draw nonplastic synapses
    int setting, index, setting_n;
    unsigned char r, g ,b, rn, gn,bn;
    for (int pre = 0; pre < N_NEURONS; pre++) {
        for (int post = 0; post < N_NEURONS; post++) {
            setting = controller.nonplastic[pre][post];
	    setting_n = controller.plastic[pre][post];
            if (setting == 0) {
                // Nothing, white
                r = 255;
                g = 255;
                b = 255;
            } else if (setting < 0) {
                // Inhibitory, blue(low) -> green (high)
                r = 0;
                g = (unsigned char) (-0.25 * (double)setting * 255);
                b = 255;
            } else {
                // Excitory, purple (low) -> red (high)
                r = 255;
                g = 0;
                b = 255 - (unsigned char) (0.25 * (double)setting * 255);
            }
	    if (setting_n == true) {
		rn = 255;
		gn = 100;
		bn = 250;
             }
	    else {
		rn = 255;
		gn = 255;
		bn = 255;
	      }

            // add to image, pre is horizontal axis, post is vertical axis
            // invert vertical axis (origin should be lower left corner)
            index = N_NEURONS * (N_NEURONS - 1 - pre) * 3 + post * 3;
            img.data[index + 0] = b;
            img.data[index + 1] = g;
            img.data[index + 2] = r;

	    img_p.data[index + 0] = bn;
	    img_p.data[index + 1] = gn;
	    img_p.data[index + 2] = rn;
        }
    }

    // Display
    cv::imshow(win, img);
    cv::waitKey();

    // convert to grayscale for plastic synapses
    cv::cvtColor(img_p, img_gray, CV_RGB2GRAY);
    cv::imshow(win, img_gray);
    cv::waitKey();
}
