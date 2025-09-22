# I/O multiplexing for concurrent connection handling

This manual uses the code from the prior lab as a basis (iterative server) to guide you towards a solution.

## Improving the server

As you may have noticed, the simple iterative server has a few flaws. Clients are handled in a serial manner, and a single client can block the server more-or-less indefinitely. 

Your next task is to turn the simple iterative server into a concurrent server using I/O multiplex-
ing with `select()`. 

Additionally, some help might be found in the source code of multi-client emulator program. The multi-client emulator also uses multiplexed I/O with `select()` to handle the large number of connections it attempts to establish.

### Non-Blocking Modes

So far, most network related operations in the iterative server are blocking, i.e. they wait for a certain event to occur, and return when this event has taken place. For example, the line

```bash
ssize_t ret = recv( cd.sock, cd.buffer, kBufferSize, 0 );
```

would wait for data to become available on the socket `cd.sock`. In a concurrent server, blocking operations must be avoided: for example, during the time the program waits for data to become available on the above socket, a new client might attempt to connect and perform its transaction.

To make sure that no operations are blocking, sockets can be put into non-blocking mode (see `set_socket_nonblocking()` in the server’s source code).Change the line.

`#define NONBLOCKING 0`
to
`#define NONBLOCKING 1`

Recompile and run the server. The server should now repeatedly print error messages. For example:

```bash
./server-concurrent
accept() failed: Resource temporarily unavailable
accept() failed: Resource temporarily unavailable
```

Since the listening socket is in non-blocking mode, attempts to call `accept()` return immediately and indicate that no new connection could be accepted (there was none!) using an error condition of `EWOULDBLOCK` or `EAGAIN` (the error conditions are equivalent and, on some systems, they are represented by the same error code.)

In the `accept()` man-page: if no messages are available at the socket, the receive call waits for a message to arrive, unless the socket is nonblocking (see fcntl(2))

```C++
//--    set_socket_nonblocking()   ///{{{1////////////////////////////////////
// set socket in NONBLOCK
static bool set_socket_nonblocking( int fd )
{
	int oldFlags = fcntl( fd, F_GETFL, 0 );
	if( -1 == oldFlags )
	{
		perror( "fcntl(F_GETFL) failed" );
		return false;
	}

	if( -1 == fcntl( fd, F_SETFL, oldFlags | O_NONBLOCK ) )
	{
		perror( "fcntl(F_SETFL) failed" );
		return false;
	}

	return true;
}
```

### Waiting for an event with SELECT()

Using `select()`  , it is possible to wait for some event (e.g. an incoming connection, or data becoming available, and so on) to occur - on one or more sockets. The `select()` method will also identify on which sockets the event(s) took place.

Start by just adding the listening socket, listenfd, to the set of file-descriptors watched for non blocking reads to become possible (named `readfds` in the man-page of select()). Also add an appropriate call to `select()`.

```C++
fd_set readfds;
FD_ZERO(&readfds);          
FD_SET(listenfd, &readfds); 
```

When `select()` returns, its return value describes the number of events that have occurred (or -1 if there was an error). Additionally, `select()` will have changed the file descriptor sets passed to it - they will only contain the file descriptors which are ready for reading (`readfds`) or writing (`writefds`). Thus, after the call to select, you can check, with FD_ISSET, if the listening socket remains a member of the `readfds` - if so, a new connection is ready to be accepted.

```C++
int nready = select(listenfd + 1, &readfds, NULL, NULL, NULL);
if (FD_ISSET(listenfd, &readfds)) {
    int connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);
}
```

Compile and run the server with these modifications. Make sure that `select()` returns when you attempt to connect to the server. The program should only call accept() when this event occurs (and `listenfd` is set in the readfds).

At this point, the server program should no longer produce the “accept() failed: Resource ...” error messages (although if you establish a connection, a different error will probably be raised).

### Tracking open connections

Currently, the server will attempt to receive from and send to a client directly after accepting the connection. Of course, that is a bad idea in a concurrent server - the server should wait until data is available for reception (and conversely, that it is possible to send data without blocking). In order to do this, the server needs to keep track of open connections.

**I think the server use select() to keep track of those fd which have build the connection just like keep track of the listenfd, wait until the data is available to be read and send**

A `ConnectionData` structure is already defined in the server - each open connection will require a copy of that structure. A convenient way to give each connection a unique copy is to dynamically allocate the structure and store it in a data structure of some kind. A simple such data structure is the linked list - if you wish, you may (re-) implement a linked list and use that. (I don't want to use that!)

```C++
struct ConnectionData
{
	EConnState state; // state of the connection; see EConnState enum

	int sock; // file descriptor of the connections socket.

	// items related to buffering.
	size_t bufferOffset, bufferSize; 
	char buffer[kTransferBufferSize+1];
};
```

Of course, if you’re lazy (you should be!), you may simply use a C++ container such as the STL `std::vector<> or std::list<>` template classes. (*I choose this way*)

```C++
// initialize connection vector
std::list<ConnectionData*> connections;
```

### More SELECT()

So far, only the listening socket is added to the file descriptor sets for select(). But, the server also needs to be notified when data can be received from or sent to any open connection.

Before calling `select()`, also insert the sockets from open connections into the correct sets. Con nections that are expected to receive data (i.e. their state is `eConnStateReceiving`) should be added to the readfds; connections that need to send data (their state is eConnStateSending) should instead be added to the writefds.

```C++
for (auto conn : connections){
			if (conn->state == eConnStateReceiving){
				FD_SET(conn->sock, &readfds);
			}
			if (conn->state == eConnStateSending){
				FD_SET(conn->sock, &writefds);
			}
			if (maxfd < conn->sock){
				maxfd = conn->sock;
			}
		}

int ret = select( maxfd + 1, &readfds, &writefds, NULL, &tv);
```

The state of new connections is initialized to `eConnStateReceiving` by the code. 

**I think the state will be initialized to `eConnStateReceiving` when initialize connection data and then put into the vector** 

Additionally, the methods `process_client_recv()` and `process_client_send()` will change the state when appropriate - that is, you don’t have to worry about that part. 

**Should depart it when deal with the Connections vector**

What you do have to worry about is *calling* the above methods at the correct times. The method for receiving data, `process_client_recv()`, should be called when data is available to be received on a given connection. The `process_client_send()` method should be called when the socket is ready for sending. Implement this.

```C++
if(conn->state == eConnStateReceiving && FD_ISSET(conn->sock, &readfds)){
	processFurther = process_client_recv( *conn );
}

if(conn->state == eConnStateSending && FD_ISSET(conn->sock, &writefds)){
	processFurther = process_client_send( *conn );
}
```

Additionally, if either method returns false, the connection should be closed (so that the socket’s file descriptor can be reused for a new connection), and the connection’s unique copy of the `ConnectionData` structure should be removed from the list of open connections. Also implement this.

**using `processFurther` as a return values.**

```C++
if(!processFurther){
	// done - close connection
	close(conn->sock);
	delete conn;
	i = connections.erase(i);
}
```

## Experiments with the concurrent server

### Exercise I.a.1

Establish two connections to the improved server using two instances of the simple client program.

Try to send messages with each of the clients. Describe the results – do you receive a response
immediately?. 

*I receive a respomse of echo immediately and both client have connection with the server;Yes*

Check with netstat and document the status of the connection from each client.

### Exercise I.a.2

Repeat the measurement with 100 clients and 10000 queries, using the multi client emulator. Compare to the timings you wrote down in the previous Lab. Compare to the timings you wrote down in the previous Lab.

### Exercise I.a.3

Consider the very simple denial of service attack we performed in the previous 












