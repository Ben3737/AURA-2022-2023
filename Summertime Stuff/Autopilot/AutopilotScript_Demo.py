print 'Start Script'
int auto_signal = -1 #pick a value not used by iBus. 
int loop = 1

while(loop == 1):
	if(Script.GetParam(chx1in) == auto_signal):
		MAV.doArm(True)
		Script.ChangeMode("RTL")
		loop = 0
