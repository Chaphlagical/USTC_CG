import cv2 as cv

img=cv.imread("ustc.png")
img=cv.resize(img,(1024,1024))
img=cv.cvtColor(img,cv.COLOR_RGB2GRAY)
img = cv.GaussianBlur(img, ksize=(31, 31), sigmaX=0, sigmaY=0)
cv.imshow("aaa",img)
cv.waitKey(0)
cv.imwrite("a.png",img)