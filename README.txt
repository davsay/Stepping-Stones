CS457
Program 2

Project members:
David Sahud
Joshua Zarin
Xi Wei

Makefile commands:
Make - creates both 'awget' and 'ss' executables
Make awget - creates only the 'awget' executable
Make ss - creates only the 'ss' executable
Make target - creates a tar file containing all source code files, makefile and readme
Make clean - deletes all executable, object files, and the tar file
Make clean clear - deletes all executable, object files, and tar files AND executes the linux 'clear' command

Program information:




awget:
	-The program must be given either 2 arguments (program name and URL) -OR- 4 arguments (program name, URL, '-c' tag, chain file name)
	-The program connects to a randomly chosen chain computer in the given chain file -OR- chaingang.txt by default (when the program is run with 2 arguments)
	-After successfully sending the chainfile to the next chain computer, the program waits for the downloaded file to be sent back over that same connection
	-The program returns 0 on success and -1, along with an error message, on failure
	-The program uses awget.h for all of its functions and library imports



ss:
	-The program must be given 3 arguments: the program name, '-p' to indicate the port is being received, and the port number on which it is waiting for a connection
	-The program will either be a final stone or a stepping stone, depending on its position in the given chainfile
	-If the program is a stepping stone, it will establish a connection and forward the information with the updated chainfile to the next server address/port in the file. After receiving the file being sent back to the client, it will close the connection to the next stepping stone and send the file to the stepping stone or client that initially connected to it.
	-If the program is a final stone, it will connect to the given URL and download the requested file. After downloading it, the file will be sent back to the stepping stone or client that connected to the final stone.
	-The program returns 0 on succes and -1, along with an error message, on failure
	-The program uses awget.h for all of its functions and library imports

chainfile(.txt):
	-first row = integer indicating the number of lines in the file
	-following rows = IP number followed by a space followed by a Port number
	-Example:
		2
		129.82.45.242 3360
		129.82.45.242 3361


