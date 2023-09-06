# Le compilateur
CC = g++

# Les flags
CFLAGS  = -g -Wall 
RM = rm
MAKEFLAGS = -Wno-file-ignored -s -j

# La cible du code 
TARGET = main
SCENE = scene
# Les scènes et images
SCENES = scenes/scene1.txt scenes/scene2.txt scenes/scene3.txt scenes/scene4.txt scenes/scene5.txt 
IMAGES = images/image1 images/image2 images/image3 images/image4 images/image5 

.SILENT:

all: $(TARGET)

# Démarche :
#	- On compile raytrace.cpp
#	- On éxécute raytrace.exe avec comme arguments la scène (en 1ère) et l'image output (en 2ème)
#	- On convertit l'image .tga en .png (Il faut avoir installé la biblio graphicsmagick)

$(TARGET): src/hpp/objects.hpp src/hpp/structs.hpp src/hpp/tga_image.hpp 
	echo " "
	
	echo "Construction des images..."
	$(CC) $(CFLAGS) -o $(TARGET) src/cpp/$(TARGET).cpp

	for i in $(SCENES); do \
		START=$$(date +%s.%N); \
		./$(TARGET) $$i images/$$(echo $$i | cut -d'/' -f2 | cut -d'.' -f1 | sed 's/scene/image/').tga; \
		END=$$(date +%s.%N);\
		DIFF=$$(echo "$$END - $$START" | bc); \
		printf "Elapsed time : %.2f s.\n" $$DIFF;\
	done
	echo ""

	$(RM) images/*.tga

	echo "Construction de la mosaïque..."
	for image in $(IMAGES); do \
		convert $$image.png -resize 1000x1000^ -gravity center -background black -extent 1000x1000 $$image"_resized.png"; \
	done

	montage images/image1_resized.png images/image2_resized.png images/image3_resized.png images/image4_resized.png -tile 2x2 -geometry +0+0 images/mosaic.png
	$(RM) images/*resized.png

	echo "Mosaïque finie!"
	eog images/mosaic.png

clean:
	$(RM)  $(TARGET) 
	$(RM)  images/*.png

image1:	
	$(CC) $(CFLAGS) -o $(TARGET) src/cpp/$(TARGET).cpp
	./$(TARGET) scenes/scene1.txt images/image1.tga
	$(RM) images/image1.tga
	eog images/image1.png

image2:	
	$(CC) $(CFLAGS) -o $(TARGET) src/cpp/$(TARGET).cpp
	./$(TARGET) scenes/scene2.txt images/image2.tga
	$(RM) images/image2.tga
	eog images/image2.png

image3:	
	$(CC) $(CFLAGS) -o $(TARGET) src/cpp/$(TARGET).cpp
	./$(TARGET) scenes/scene3.txt images/image3.tga
	$(RM) images/image3.tga
	eog images/image3.png

image4:	
	$(CC) $(CFLAGS) -o $(TARGET) src/cpp/$(TARGET).cpp
	./$(TARGET) scenes/scene4.txt images/image4.tga
	$(RM) images/image4.tga
	eog images/image4.png

image5:	
	$(CC) $(CFLAGS) -o $(TARGET) src/cpp/$(TARGET).cpp
	./$(TARGET) scenes/scene5.txt images/image5.tga
	$(RM) images/image5.tga
	eog images/image5.png

scene:
	$(CC) $(CFLAGS) -o $(TARGET) src/cpp/$(TARGET).cpp
	./$(TARGET) scenes/scene6.txt images/image6.tga
	$(RM) images/image6.tga;
	eog images/image6.png