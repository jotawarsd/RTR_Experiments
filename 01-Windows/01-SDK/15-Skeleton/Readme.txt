New User defined Functions:-
	int initialize(void)
	void resize(int width, int height):
		here, if height ever becomes 0, it is assigned 1 because in future programs, when height is divided by zero, it creates an illegal instruction.

	void display(void)
	void update(void)
	void uninitialize(void):
		-Instead of putting the code for closing the log file under WM_DESTROY, we put this code in uninitialize and uninitialize is called under WM_DESTROY
		-DestroyWindow(ghwnd) is called because if uninitialize is called somewhere else, then window won't be destroyed
		-if window is in fullscreen, then 
		
New Library Functions:-
	SetForegroundWindow()
	SetFocus()

New Messages and handlers:- 
	WM_SIZE
		This message contains the size of the window in its LPARAM
		width = LOWORD(lParam)		
		height = LOWORD(lParam)
