# Read image and display in openCV window

import argparse
import cv2

parser = argparse.ArgumentParser()
parser.add_argument('--image', default='tetris_blocks.png',
                    help='Image path.')
params = parser.parse_args()

img = cv2.imread(params.image)
window_title = "Image Name: " + params.image
cv2.imshow(window_title, img)
cv2.waitKey(0)
