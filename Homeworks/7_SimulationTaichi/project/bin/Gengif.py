import imageio
import os
import sys

def create_gif(source,name,duration):
	frames=[]
	for image in source:
		frames.append(imageio.imread(image))
	imageio.mimsave(name,frames,'GIF',duration=duration)
	print("Finish")

def main(path):
	pic_list=os.listdir(path)
	temp_list=[]
	for file in pic_list:
		temp_list.append(os.path.join(path,file))
	pic_list=temp_list
	gif_name="result.gif"
	duration_time=0.0
	create_gif(pic_list,gif_name,duration_time)

if __name__=='__main__':
	param_list=sys.argv
	if len(param_list)!=2:
		print("Input the folder!")
	else:
		main(param_list[1])
