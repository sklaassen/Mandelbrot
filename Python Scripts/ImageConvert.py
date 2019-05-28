import imageio
imageio.plugins.ffmpeg.download()
from PIL import Image
from moviepy import editor


path='C:\\Users\\Stefan\\Documents\\CodeStuff\\ImageStuff\\pictures\\'
clip = editor.ImageSequenceClip(path,fps=30)
clip.write_videofile("movie.mp4",fps=30)

#if you want to make a gif
#clip.write_gif("movie2.gif",fps=24)

#if you want to convert an image
#im = Image.open(path+"0000.ppm")
#im.save("e.png")
