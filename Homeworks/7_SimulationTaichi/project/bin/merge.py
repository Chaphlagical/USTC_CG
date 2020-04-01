import cv2 as cv
import os
import sys
import numpy as np

p1='D:\\USTC_CG\\Homeworks\\7_SimulationTaichi\\project\\bin\\rectangle_shoot'

path1=os.listdir(p1)
pic1_list=[]
for file in path1:
	pic1_list.append(os.path.join(p1,file))

p2='D:\\USTC_CG\\Homeworks\\7_SimulationTaichi\\project\\bin\\circle_shoot'

path2=os.listdir(p2)
pic2_list=[]
for file in path2:
	pic2_list.append(os.path.join(p2,file))

p3='D:\\USTC_CG\\Homeworks\\7_SimulationTaichi\\project\\bin\\triangle_shoot'

path3=os.listdir(p3)
pic3_list=[]
for file in path3:
	pic3_list.append(os.path.join(p3,file))

p4='D:\\USTC_CG\\Homeworks\\7_SimulationTaichi\\project\\bin\\flat_shoot'

path4=os.listdir(p4)
pic4_list=[]
for file in path4:
	pic4_list.append(os.path.join(p4,file))

font = cv.FONT_HERSHEY_SIMPLEX
for i in range(len(pic1_list)):
	img1=cv.imread(pic1_list[i])
	img2=cv.imread(pic2_list[i])
	img3=cv.imread(pic3_list[i])
	img4=cv.imread(pic4_list[i])

	#img1=cv.putText(img1, 'partical num:500', (100, 100), font, 1.2, (255, 255, 255), 2)
	#img2=cv.putText(img2, 'partical num:5000', (100, 100), font, 1.2, (255, 255, 255), 2)
	#img3=cv.putText(img3, 'partical num:50000', (100, 100), font, 1.2, (255, 255, 255), 2)
	#img4=cv.putText(img4, 'partical num:500000', (100, 100), font, 1.2, (255, 255, 255), 2)


	img=np.zeros([img1.shape[0]*2,img1.shape[1]*2,3])
	img[0:img1.shape[0],0:img1.shape[1]]=img1
	img[0:img2.shape[0],img1.shape[1]:img2.shape[1]*2]=img2
	img[img3.shape[0]:img3.shape[0]*2,0:img3.shape[1]]=img3
	img[img3.shape[0]:img3.shape[0]*2,img3.shape[1]:img3.shape[1]*2]=img4
	cv.imwrite('D:\\USTC_CG\\Homeworks\\7_SimulationTaichi\\project\\bin\\shape_shoot\\{:05d}.png'.format(i),img)
	cv.imshow("1",img1)
	cv.imshow("2",img2)
	cv.imshow("3",img3)
	cv.imshow("4",img)
	cv.waitKey(1)