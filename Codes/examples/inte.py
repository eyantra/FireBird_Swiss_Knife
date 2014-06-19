exec(open("functionList.py").read());

init("COM5");

def test():
	print("tick");
	
onInterrupt("left",test);
while 1==1:
	2==2;