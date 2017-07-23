Problem Statement

SERVER
1. Client send command to the server over socket
2. Server will create new thread to process each client requests.
3. Server Sends Results back to server after processing request.
4. Server saves each command and results in file.

CLIENT
1. Client connect to server over ip and TCP port
2. On successfull connection client user will get CLI prompt
3. Client will have following commands available
	- help 
	- fibonacci [number]
	- sort [space seperated array numbers]
	- random [num1] [num2]
	- Show History
	- exit   
4. After successfull execution of command, results will get displayed on client side 
5. Multiple client can connect to server at time.
