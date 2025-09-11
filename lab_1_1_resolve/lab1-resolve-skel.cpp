/********************************************************* -- SOURCE -{{{1- */
/** Translate host name into IPv4
 *
 * Resolve IPv4 address for a given host name. The host name is specified as
 * the first command line argument to the program. 
 *
 * Build program:
 *  $ g++ -Wall -g -o resolve <file>.cpp
 */
/******************************************************************* -}}}1- */

#include <stdio.h>
#include <stddef.h>

#include <assert.h>
#include <limits.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

//--//////////////////////////////////////////////////////////////////////////
//--    local declarations          ///{{{1///////////////////////////////////

void print_usage( const char* aProgramName );

//--    local config                ///{{{1///////////////////////////////////

/* HOST_NAME_MAX may be missing, e.g. if you're running this on an MacOS X
 * machine. In that case, use MAXHOSTNAMELEN from <sys/param.h>. Otherwise 
 * generate an compiler error.
 */
#if !defined(HOST_NAME_MAX)
#	if defined(__APPLE__)
#		include <sys/param.h>
#		define HOST_NAME_MAX MAXHOSTNAMELEN
#	else  // !__APPLE__
#		error "HOST_NAME_MAX undefined!"
#	endif // ~ __APPLE__
#endif // ~ HOST_NAME_MAX

//--    main()                      ///{{{1///////////////////////////////////
int main( int aArgc, char* aArgv[] )
{
	// Check if the user supplied a command line argument.
	if( aArgc != 2 )
	{
		print_usage( aArgv[0] );
		return 1;
	}

	// The (only) argument is the remote host that we should resolve.
	const char* remoteHostName = aArgv[1];

	// Get the local host's name (i.e. the machine that the program is 
	// currently running on).
	const size_t kHostNameMaxLength = HOST_NAME_MAX+1;
	char localHostName[kHostNameMaxLength];


	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	// specify AF_INET as the address family
	hints.ai_family = AF_INET;
	// set the socket type to SOCK_STREAM
	hints.ai_socktype = SOCK_STREAM;
	// set the protocol to IPPROTO_TCP
	hints.ai_protocol = IPPROTO_TCP;
	// set the remaining elements to zero

	struct addrinfo* result = nullptr;

	if( -1 == gethostname( localHostName, kHostNameMaxLength ) )
	{
		perror( "gethostname(): " );
		return 1;
	}

	// Print the initial message
	printf( "Resolving `%s' from `%s':\n", remoteHostName, localHostName );

	// TODO : add your code here
	// the service argument (the second argument), set to NULL – we’re not interested in
    // any specific port or service
	// check the return value for errors
	int errors = getaddrinfo(remoteHostName, NULL, &hints, &result);
	if(0 != errors){
		// use gai_strerror() to convert the returned error
        // code to a human-readable string
		fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(errors));
		return 1;
	}
	
	if (result == nullptr) {
		printf("getaddrinfo returned no results!\n");
		return 1;
	}
	for(struct addrinfo* p = result; p != nullptr; p = p -> ai_next){
		assert(p->ai_family == AF_INET);
		struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr; // cast to sockaddr_in*
		
		// to convert an in_addr to a human-readable string,
        // you can use inet_ntop()
		char ipStr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(ipv4->sin_addr), ipStr, sizeof(ipStr));
		printf("IPv4: %s\n", ipStr);
	} 

	freeaddrinfo(result);

	// Ok, we're done. Return success.
	return 0;
}


//--    print_usage()               ///{{{1///////////////////////////////////
void print_usage( const char* aProgramName )
{
	fprintf( stderr, "Usage: %s <hostname>\n", aProgramName );
}

//--///}}}1/////////////// vim:syntax=cpp:foldmethod=marker:ts=4:noexpandtab: 
