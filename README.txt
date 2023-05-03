Errors:
	1. After user presses Enter key to confirm command, there would an extra '\n' char, 
	hence on consecutive user inputs the '\n' will be read and not wait for user input.
	
	2. System shuts down after exiting/killing more than 1 processes(excluding init).

	3. In Totalinfo(), all ready processes appear in all the ready queues.

	4. Reply() sends a reply and unblocks sender, however the 'replied' message is not displayed. 
	Sometimes reply crashes the system.

	5. Given list. & list.o don't work. Undefined reference to `__assert_fail' in functions
	`makeNewNode', `linkNodeAfterCurrent', `isOOBAtStart' & `isOOBAtEnd'

Debug log:
	1. Tried using fflush() however it did not fix the problem. 
	Using getchar() != '\n'in a loop to clear input stream.

	2, 3, 4. Debugging list.c to resolve errors.

References:
	1. https://stackoverflow.com/questions/7898215/how-to-clear-input-buffer-in-c