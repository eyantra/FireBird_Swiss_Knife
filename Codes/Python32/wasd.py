exec(open("functionList.py").read());
from msvcrt import getch
import time

init("COM5");

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
		buzzerOn();	
	time.sleep(.01);
	stop();
	buzzerOff();	
	print (z)