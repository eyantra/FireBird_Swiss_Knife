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
def setPort(portName,value):
	import zigbee;
	zigbee.sendString("setPort#");
	zigbee.sendString(portName);
	zigbee.sendString("#");
	zigbee.sendString(str(value));
	zigbee.sendString("#");
def getPort(portName):
	import zigbee;
	zigbee.sendString("getPort#");
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
	zigbee.sendString("getRightWRS#");
	return int(zigbee.getString());
def getCenterWLS():
	import zigbee;
	zigbee.sendString("getCenterWLS#");
	return int(zigbee.getString());
def setVelocity(x,y):
	import zigbee;
	zigbee.sendString("setVelocity#");
	zigbee.sendString(x);
	zigbee.sendString("#");
	zigbee.sendString(y);
	zigbee.sendString("#");

