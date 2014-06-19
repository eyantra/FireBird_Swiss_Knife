import _thread
def init(port):
	import zigbee;
	zigbee.init(port);
def forward():
    import zigbee;
    zigbee.sendString("w#");
def stop():
    import zigbee;
    zigbee.sendString(" #");
def backward():
    import zigbee;
    zigbee.sendString("s#");
def left():
    import zigbee;
    zigbee.sendString("a#");
def right():
    import zigbee;
    zigbee.sendString("d#");
def buzzerOn():
    import zigbee;
    zigbee.sendString("h#");
def buzzerOff():
    import zigbee;
    zigbee.sendString("m#");
def lcdString(x):
	import zigbee;
	zigbee.sendString("lcd#");
	zigbee.sendString(x);
	zigbee.sendString("#");
def getString():
	import zigbee;
	return zigbee.getString();
def sendString(x):
	import zigbee;
	zigbee.sendString(str(x));
	zigbee.sendString("#");
def setPort(portName,value):
	import zigbee;
	zigbee.sendString("setPort#");
	zigbee.sendString(portName);
	zigbee.sendString("#");
	zigbee.sendString(str(value));
	zigbee.sendString("#");
def getPin(portName):
	import zigbee;
	zigbee.sendString("getPin#");
	zigbee.sendString(portName);
	zigbee.sendString("#");
	return int(zigbee.getString());
def strictForward():
	import zigbee;
	zigbee.sendString("strictForward#");
def strictBack():
	import zigbee;
	zigbee.sendString("strictBack#");
def moveOnArc(radius,dir):
	import zigbee;
	zigbee.sendString("moveOnArc#");
	zigbee.sendString(str(radius));
	zigbee.sendString("#");
	zigbee.sendString(str(dir));
	zigbee.sendString("#");	
def rollLcd(data):
	import zigbee;
	zigbee.sendString("rollLCD#");
	zigbee.sendString(data);
	zigbee.sendString("#");
def getLeftWLS():
	import zigbee;
	zigbee.sendString("getLeftWLS#");
	return int(zigbee.getString());
def getRightWS():
	import zigbee;
	zigbee.sendString("getRightWLS#");
	return int(zigbee.getString());
def getCenterWLS():
	import zigbee;
	zigbee.sendString("getCenterWLS#");
	return int(zigbee.getString());
def setVelocity(x,y):
	import zigbee;
	zigbee.sendString("setVelocity#");
	zigbee.sendString(str(x));
	zigbee.sendString("#");
	zigbee.sendString(str(y));
	zigbee.sendString("#");
def listenForInterrupt(interruptName):
	import zigbee;
	zigbee.sendString("listenForInterrupt#");
	zigbee.sendString(interruptName);
	zigbee.sendString("#");
	return int(zigbee.getString());
def interruptHandler(interruptName,func,delay):
	import time;
	while(1==1):
		y=listenForInterrupt(interruptName);
		for i in range(0,y):
			func();	
		time.sleep(delay);		
def onInterrupt(interruptName,func,delay=.15):
	import zigbee;
	zigbee.sendString("resetInterrupt#");
	zigbee.sendString(interruptName);
	zigbee.sendString("#");
	_thread.start_new_thread(interruptHandler,(interruptName,func,delay,));	
def getIRSharp(num):
	import zigbee;
	zigbee.sendString("getIRSharp#");
	zigbee.sendString(str(num));
	zigbee.sendString("#");
	return int(zigbee.getString());
def getIRProx(num):
	import zigbee;
	zigbee.sendString("getIRProx#");
	zigbee.sendString(str(num));
	zigbee.sendString("#");
	return int(zigbee.getString());