rapache
=======
Rapache is a simple Common Gateway Interface(CGI) webserver written in bash.

How to use
-------
1. Install `ucspi-tcp` 
2. Clone `rapache` to your machine.
3. Enter `rapache` directory.
4. Run `./tcpserver.sh` to start server.
```
$ sudo apt-get install ucspi-tcp
$ git clone https://github.com/Dongdongshe/rapache
$ cd rapache
$ ./tcpserver.sh
```

Examples
---------
Run server using tcpserver which can handle multiple connections
```
./tcpserver.sh
```
Run server using netcat which response to one connection a time.
```
./netcat.sh
```

Feature
-----------
1. Rapache supports HTTP GET and POST methods.
2. Rapache implements two ways(netcat or tcpserver) to handle tcp connections.
3. Rapache runs in daemon mode.

