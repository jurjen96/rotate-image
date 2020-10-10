# Rotate image via recursion

Rotate an image via dividing the image into smaller segments each 
recursion and apply translation to those segments.  

![Example, credits image: rhonddak](bin/example.gif)

## Idea
When scrolling through Reddit, I saw the post of user u/Agha_shadi 
show casing an image of Albert Einstein being rotated without rotating
the segments, but only applying translation to the segments. 
([Original Post](https://www.reddit.com/r/blackmagicfuckery/comments/j4cn6q/recursively_rotating_segments_of_an_image_rotates/)).
I'm currently trying to learn the C++ language, so I thought it would 
be a nice challenge to create something similair in C++. After a lot 
of trail and error, I finished my first C++ program :) If you know a 
way to improve the code or have just general feedback; please open an 
issue with your feedback so that I can adjust the code and learn from 
it!

## Usage
Clone the project:
```
git clone https://github.com/jurjen96/rotate-image
```
Go into the just created folder and start the program via:
```
cd rotate-image/bin
./main name_of_an_image.jpg
```
After a minute or so, depending on the size and the CPU resources 
available, the program should notify you that it is done and that 
the results are stored in the same folder you exuted the `main` 
file from. Open `results.avi` and it should show you an image being
rotated by only applying translation :)

## Limitation
The program currently only supports images up to 1024px. Images larger 
than 1024px will be downscaled to 1024px. Also, the image should be a 
square, the width should equal the height.

## Credits:

Inspiration:
https://github.com/rvizzz/rotate

Image: 
rhonddak: https://unsplash.com/photos/_Yc7OtfFn-0
