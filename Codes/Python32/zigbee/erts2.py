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
	print(type(portName));
	zigbee.sendString(portName);
	zigbee.sendString("#");
	x=zigbee.getString();	
	while(bytes.decode(x)!=portName):
		print(" in loop ");		
		print(x);
		x=zigbee.getString();
	return zigbee.getString()