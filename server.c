/* Server side - UDP Code				    */
/* By Hugh Smith	4/1/2017	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "gethostbyname.h"
#include "networks.h"
#include "safeUtil.h"
#include "checksum.h"
#include "cpe464.h"
#include "pdu.h"

#define MAXBUF 80

void processClient(int socketNum);
int checkArgs(int argc, char *argv[]);

int main ( int argc, char *argv[]  )
{ 
	int socketNum = 0;				
	int portNumber = 0;

	portNumber = checkArgs(argc, argv);
	
	char* ER = argv[1];
	double errorRate = atof(ER);
	
	sendtoErr_init(errorRate, DROP_OFF, FLIP_ON, DEBUG_ON, RSEED_OFF);

	socketNum = udpServerSetup(portNumber);

	processClient(socketNum);

	close(socketNum);
	
	return 0;
}

void processClient(int socketNum)
{
	int dataLen = 0; 
	char buffer[MAXBUF + 1 + 7];	  
	struct sockaddr_in6 client;		
	int clientAddrLen = sizeof(client);	
	int seqNum = 1;
	
	buffer[0] = '\0';
	while (buffer[0] != '.')
	{
		dataLen = safeRecvfrom(socketNum, buffer, MAXBUF, 0, (struct sockaddr *) &client, &clientAddrLen);
	
		printf("Received message from client with ");
		printIPInfo(&client);
		//printf(" Len: %d \'%s\'\n", dataLen, buffer);
		printPDU((uint8_t *) buffer, dataLen);

		// just for fun send back to client number of bytes received
		sprintf(buffer, "bytes: %d", dataLen);
		uint8_t newPDU[MAXBUF + 1 + 7];
		int sendLen = createPDU(newPDU, seqNum, 3, (uint8_t *) buffer, strlen(buffer) + 1);

		safeSendto(socketNum, newPDU, sendLen, 0, (struct sockaddr *) & client, clientAddrLen);
		seqNum += 1;

	}
}

int checkArgs(int argc, char *argv[])
{
	// Checks args and returns port number
	int portNumber = 0;

	if (argc > 3)
	{
		fprintf(stderr, "Usage %s [optional port number]\n", argv[0]);
		exit(-1);
	}
	
	if (argc == 3)
	{
		portNumber = atoi(argv[2]);
	}
	
	return portNumber;
}


