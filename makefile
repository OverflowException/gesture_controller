#To build executable for raspberry pi, run
#	make RPI=1

TARGET_1 = bin/caliberator

CXX = g++
INC = `pkg-config --cflags opencv` -I./include #and other include paths -I

ifdef RPI
LIB_RPICAM = -lraspicam_cv
DEBUG = -DRPI_CAM
else
LIB_RPICAM =
DEBUG = 
endif
LIB = `pkg-config --libs opencv` $(LIB_RPICAM) #and other lib paths and lib names -L -l

CFLAGS = -Wall -c -std=c++11
LFLAGS = -Wall -pthread

COMPILE = $(CXX) $(CFLAGS) $(INC) $(DEBUG) $^ -o $@
LINK = $(CXX) $(LFLAGS) $(LIB) $^ -o $@


.PHONY: all
all: $(TARGET_1) #and other targets

$(TARGET_1): build/caliberator.o build/histgenerator.o build/sampler.o
	$(LINK)
build/histgenerator.o: src/histgenerator.cpp
	$(COMPILE)
build/sampler.o: src/sampler.cpp
	$(COMPILE)
build/caliberator.o: src/caliberator.cpp
	$(COMPILE)


#[other targets]

$(shell mkdir -p bin)
$(shell mkdir -p build)

clean:
#	rm -frv *.o
	rm -frv ./build/*.o
