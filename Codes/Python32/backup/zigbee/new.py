from msvcrt import getch
import time

def init(port):
	import zigbee;
	zigbee.init(port);
def forward():
    import zigbee;
    zigbee.sendString("w");
def stop():
    import zigbee;
    zigbee.sendString(" ");
def backward():
    import zigbee;
    zigbee.sendString("s");
def left():
    import zigbee;
    zigbee.sendString("a");
def right():
    import zigbee;
    zigbee.sendString("d");
def buzzer_on():
    import zigbee;
    zigbee.sendString("h");
def buzzer_off():
    import zigbee;
    zigbee.sendString("m");
	
init("COM6");
while True:
	z = getch();
	if ord(z) == 27:
		break
	if ord(z)== ord('w'):
		print("forward");
		forward();
	if ord(z)==ord('s'):
		backward();
	if ord(z)==ord('a'):
		left();
	if ord(z)==ord('d'):
		right();
	if ord(z)==ord(' '):
		stop();
	if ord(z)==ord('h'):
		buzzer_on();	
	time.sleep(.01);
	stop();
	buzzer_off();	
	print (z)