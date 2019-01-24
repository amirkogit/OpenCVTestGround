import cv2
import pydicom
import matplotlib.pyplot as plt

from pydicom.data import get_testdata_files
filename = get_testdata_files("CT_small.dcm")[0]
ds = pydicom.dcmread(filename)

# plt.imshow(ds.pixel_array, cmap=plt.cm.bone)
# plt.waitforbuttonpress()
# image = cv2.imread("jp.png")

cv2.imshow("Image", ds.pixel_array)
cv2.waitKey(0)
