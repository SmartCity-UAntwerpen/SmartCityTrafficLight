CC=/home/robotdev/rpi/tools/arm-bcm2708/arm-bcm2708hardfp-linux-gnueabi/bin/arm-bcm2708hardfp-linux-gnueabi-gcc
APP_BINARY=SmartTrafficLight
APP_ARGS=
REMOTE_SERVER=pi@tlight1
REMOTE_PATH=/home/pi
REMOTE_PASS='raspberry'
ML=/home/robotdev/minimod/Libs/
ML_ROB=/home/robotdev/SmartCity/lib/
LOGFILE='log.csv'

CFLAGS += -g -DLINUX -Wall -I$(ML) -I$(ML_ROB) -I/home/robotdev/rpi-cp/usr/local/include -I.
LFLAGS += -lpthread -L$(ML) -L/home/robotdev/rpi-cp/usr/local/lib -lrt -lpiface-1.0

OBJECTS=main.o robotapp.o $(ML_ROB)timestep.o $(ML_ROB)TimeSupport.o $(ML)lin-rpi-serlib.o $(ML)crc.o $(ML)lin-delay.o $(ML)lin-gpio.o $(ML)ansi.o
all: SmartTrafficLight
Debug: SmartTrafficLight
SmartTrafficLight: $(OBJECTS)
	$(CC) $(CFLAGS) $(LFLAGS) $(OBJECTS) -o $(APP_BINARY)

-include $(OBJECTS:.o=.d)

%.o: %.c
	$(CC) -c $(CFLAGS) $*.c -o $*.o
	$(CC) -MM $(CFLAGS) $*.c > $*.d

cleanDebug: clean
clean:
	rm -f $(OBJECTS)
	rm -f $(OBJECTS:.o=.d)
	rm -f $(APP_BINARY)
install:
	chmod +x $(APP_BINARY)
	sshpass -p $(REMOTE_PASS) scp -o StrictHostKeyChecking=no  $(APP_BINARY) $(REMOTE_SERVER):$(REMOTE_PATH)
rundebug:
	sshpass -p $(REMOTE_PASS) ssh -t $(REMOTE_SERVER)  "sudo gdbserver localhost:5000 $(REMOTE_PATH)/$(APP_BINARY) $(APP_ARGS)"
runnodebug:
	sshpass -p $(REMOTE_PASS) ssh -t $(REMOTE_SERVER) sudo $(REMOTE_PATH)/$(APP_BINARY) $(APP_ARGS)
botTerminal:
	/home/robotdev/SmartCity/trafficLightTerminal.sh tlight1
plotlog:
	rm -f $(LOGFILE)
	sshpass -p $(REMOTE_PASS) scp -o StrictHostKeyChecking=no  $(REMOTE_SERVER):$(REMOTE_PATH)/$(LOGFILE) .
	octave --persist logplot.m
plotdelete:
	sshpass -p $(REMOTE_PASS) ssh -t $(REMOTE_SERVER) rm -f $(REMOTE_PATH)/$(LOGFILE)


