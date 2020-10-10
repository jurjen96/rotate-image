#include <opencv2/highgui.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/videoio.hpp> 
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <list>
#include <vector>
#include <math.h> 
#include <cmath> 

using namespace std;
using namespace cv;

using std::list;

void move_frames(Mat &image, int shift, int x, int y, int width);
void rotate_img(Mat &image, int nr_frames, int width);
void join_threads(std::vector<std::thread> &threads);
void write_image(Mat &img);
void write_image(Mat &img, int fps, int time);

std::mutex m;
Mat tmpHead;

const int fps = 30;

VideoWriter out_capture("results.avi", VideoWriter::fourcc('M','J','P','G'), fps, Size(1024,1024));

int main(int argc, char *argv[])
{
	if (argc < 2) {
		cout << "Usage ./main path/to/image.jpeg" << endl;
		return 0;
	}

	Mat src = imread(argv[1],1);

	if (src.empty()) {
		cerr << "Image not found!" << endl;
		return 0;
	}

	int width = src.cols;
	int height = src.rows;

	// Check if we deal with a square image
	if (width != height) {
		cerr << "The width of the image should equal the height! (= a square image)"<< endl;
		return 0;
	}

	// Use a bit-wise operator to check if the width is a power of 2 value
	if ((width & (width-1)) != 0) {
		int power = 1;
		// Get the closest floored power to 2 value and 
		// resize the image  to that value
		// The max supported format is 1024 x 1024 px
		while (power < width/2 && power<1024)
			power *= 2;
		resize(src, src, Size(power, power));
	} else if (width > 1024) {
		resize(src, src, Size(1024, 1024));
	}

	width = src.cols;
	height = src.rows;

	cout << "Loaded image, height: " << height << ", width: " << width << endl;

	int nr_frames = 2*log2(width/2); // y=a^x -> x = log_a(y)

	write_image(src, fps, 1);

	// Rotate the image 4 times (360 degrees)
	for (int i=0; i<4; i++) {
		rotate_img(src, nr_frames, width);
		write_image(src, fps, 1); // Add a pause of one second in video before next rotation
	}

	cout << "Done! Check the results.avi file in your folder" << endl;

	return 0;
}

/**
 * Rotate the image 90 degrees clock-wise via moving blocks of pixels (segments) around. 
 * This function uses threads to speed up the process.
 * 
 * image: a reference to the image we should perfom the rotation on
 * nr_frames: the amount of steps between each frame moving from one position to the other
 * width: the width of a segment that we should perform the translation movement on
 */
void rotate_img(Mat &image, int nr_frames, int width) {
	if (width <= 1)
		return;

	std::vector<std::thread> threads;

	for (int i=0; i<nr_frames; i++) {
		int shift = ((width/2) * (i+1)) / nr_frames;
		image.copyTo(tmpHead);

		for (int x=0; x < image.cols ; x+=width) {
			for (int y=0; y < image.rows ; y+=width) {
				if (threads.size() > 32) {
					join_threads(threads);
				}
				threads.emplace_back(std::thread(move_frames, std::ref(tmpHead), shift, x, y, width));
			}
		}

		join_threads(threads);
		write_image(tmpHead);


		// If you want to see the intermediate results, uncomment the following lines:
		// imshow("Mountain", tmpHead);
		// waitKey(1);
	}
	tmpHead.copyTo(image);
	
	width /= 2;
	nr_frames -= 2;

	rotate_img(image, nr_frames, width);
}

/**
 * Split a single segment in 4 zones and move these zones clock-wise
 * 
 * Zones are defined as:   | After a full rotation:
 *                         |
 *      0 | 1              |      2 | 0
 *     ---|---             |     ---|---
 *      2 | 3              |      3 | 1
 *  
 * image: a reference to the image we should perfom the rotation on
 * shift: the step size in the translation of the 4 zones 
 * x: defines the starting x position of the segment
 * y: defines the starting y position of the segment
 * width: the width of a single segment
 */
void move_frames(Mat &image, int shift, int x, int y, int width) {
	Mat tmp;
	image.copyTo(tmp);

	for (int i=0; i<4; i++) {
		
		int x_start, y_start; // Defines the start position of a zone
		int x_end, y_end; // Defines the position we should move to

		if (i%3 == 0) {
			x_start = x + i%2 * (width/2);
			y_start = y + ((i/2)%2) * (width/2);
			x_end = x_start + shift * (1-2*(i/3));
			y_end = y_start;
		} else {
			x_start = x + i%2 * (width/2);
			y_start = y + ((i/2)%2) * (width/2);
			x_end = x_start;
			y_end = y_start + shift * (1-2*(i/2));
		}

		Rect roi_src(x_start, y_start, width/2, width/2);
		Rect roi_target(x_end, y_end, width/2, width/2);
		//m.lock(); // Mutex lock does not seem to be necessary.
		tmp(roi_src).copyTo(image(roi_target));
		//m.unlock();
	}
}

/**
 * Wait till all threads are done with their task and then clear
 * the list of threads.
 * 
 * threads: a reference to a vector containing threads that are movig zones in segments arounds
 */
void join_threads(std::vector<std::thread> &threads) {
	for (auto& th : threads) {
		th.join();
	}
	threads.clear();
}

/**
 * Write a single image to the output video
 * 
 * img: a reference to the image that should be written to the output video
 */ 
void write_image(Mat &img) {
	out_capture.write(img);
}

/**
 * Write an image to the video output for a certain amount of time
 * this function can be used to create a pause between moving frames 
 *  
 * img: a reference to the image that should be written to the output video
 * fps: the fps of the output video
 * time: the time an image should be written in seconds
 */
void write_image(Mat &img, int fps, int time) {
	for (int j=0; j<(fps*time); j++) {
		out_capture.write(img);
	}
}