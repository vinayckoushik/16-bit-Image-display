# 16-bit-Image-display
PPM image to display on the Window using Bit Manipulation
Step1 : Parse the header to detect whether it is a RGB or Grayscale image.
Step2 : Bit-manipulation for each image to convert it into 16-bit display form. This involves taking only the higher bits of
        R,G and B values to be a total of 16-bits. This involves a 5-5-5 conversion from each colour with a redundant bit in the linux
        version of the code and a 5-6-5 conversion from R,G,B respectively in the windows version.
Step3 : Creating a window and displaying the bit manipulated image
