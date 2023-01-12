# edge-detection

computer vision algorithms for edge detection in images:
- `canny.cpp`: canny edge detection
- `hysteresis.cpp`: hysteresis edge tracking (+added command line arguments for easier useability)
- `nonmax-suppression.cpp`: non-maximum suppression edge thinning technique

## canny edge detection

- apply gaussian filter to smooth the image in order to remove the noise (converts the image to grayscale)
- find the intensity gradients of the image by applying a "mask"
- apply gradient magnitude thresholding or lower bound cut-off suppression to find edges

## hysteresis edge tracking

- command line arguments:
    - `-f`: .ppm image file path
    - `-l`: lower threshold
    - `-h`: higher threshold
- apply double threshold to determine potential edges
- finalize the detection of edges by suppressing all the other edges that are weak and not connected to strong edges with a recursive areafill algorithm

## non-maximum suppression

lower bound cut-off suppression is applied to find the locations with the sharpest change of intensity value

for each pixel in the gradient image:
- compare the edge strength of the current pixel with the edge strength of the pixel in the positive and negative gradient directions
    - (e.g., a pixel that is pointing in the y-direction will be compared to the pixel above and below it in the vertical axis)
- if the edge strength of the current pixel is the largest compared to the other pixels in the mask with the same direction, the value will be preserved
- if the edge is present after both non-max suppression and hysteresis has been applied, it is considered a "final edge"

## results:

original image:
![image](https://github.com/elemelonwind/edge-detection/blob/main/image.jpg?raw=true)

converted to grayscale (gaussian filter):
![grayscale](https://github.com/elemelonwind/edge-detection/blob/main/grayscale.jpg?raw=true)

canny edge detection algorithm:
![canny](https://github.com/elemelonwind/edge-detection/blob/main/canny.jpg?raw=true)

hysteresis edge tracking (refine + remove noisy edges):
![hysteresis](https://github.com/elemelonwind/edge-detection/blob/main/hysteresis.jpg?raw=true)

non-maximum suppression (edge thinning):
![suppression](https://github.com/elemelonwind/edge-detection/blob/main/suppression.jpg?raw=true)