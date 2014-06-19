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
def buzzer_on():
    import zigbee;
    zigbee.sendString("h#");
def buzzer_off():
    import zigbee;
    zigbee.sendString("m#");
def lcd_string(x):
	import zigbee;
	zigbee.sendString("lcd#");
	zigbee.sendString(x);
	zigbee.sendString("#");
	

	

