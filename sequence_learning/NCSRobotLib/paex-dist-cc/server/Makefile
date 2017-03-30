
#Load device-specific code
DEVICE = MN256R1

#This is where static and dynamic web content goes -- must be an absolute path
WWWDIR = /var/www/aex



#Do not edit below this line

WDIR = $(shell pwd)/build
CFLAGS = -DLOCAL_RESOURCE_PATH='"$(WWWDIR)"' -DDEVICE_$(DEVICE) -DBUILD_PATH='"$(WDIR)"'

all:
	#mkdir -p $(WWWDIR)/experiments
	#mkdir -p $(WWWDIR)/biases
	#mkdir -p build/data
	#cp resources/_biases_default.datx build/data
	#cp -r resources/static $(WWWDIR)
	#cp -r resources/experiments/* $(WWWDIR)/experiments
	#cp -r resources/biases/* $(WWWDIR)/biases
	gcc $(CFLAGS) lib/cjson/cJSON.c lib/aerctl/aerctl.c server.c -lm -lwebsockets -o build/server
