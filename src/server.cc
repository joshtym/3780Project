/* Creates a datagram server.  The port 
   number is passed as an argument.  This
   server runs forever */

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <vector>

#include <iostream>

using namespace std; 

static int paySize = 256;

enum MType {SEND = 0, GET = 1, ACK = 2}; 

int zeroDelve(long int x)
{
	long int result = x;
	int numDivs = 0;
	
	if ((result / 10) == 0)
		return 1;
	else
	{
		result /= 10;
		numDivs++;
		numDivs+= zeroDelve(result);
		
		return numDivs;
	}
}

struct Message
{
	Message(int seq = 1, MType t = SEND, long int src = 1111111111, long int des = 2222222222, char p[256] = NULL) :  seqNum(seq), mType(t), source(src), dest(des)
	{
		for(int i = 0; i < paySize; i++)
			payload[i] = ' ';
		
		if (p != NULL)
			for(int i = 0; i < paySize; i++)
				payload[i] = p[i];
	}
	
	Message( const Message& other) :  seqNum(other.seqNum), mType(other.mType), source(other.source), dest(other.dest)
	{
		if (other.payload != NULL)
			for(int i = 0; i < paySize; i++)
				payload[i] = other.payload[i];
	}
	
	Message& operator=( const Message& other)
	{
		seqNum = other.seqNum;
		mType = other.mType;
		source = other.source;
		dest = other.dest;
		
		for(int i = 0; i < paySize; i++)
			payload[i] = other.payload[i];
	}
	
	void print()
	{
		cout << "Sequence number: " << seqNum << endl;
		cout << "Type: ";
		
		switch (mType)
		{
			case 0:
				cout << "SEND" << endl;
				break;
			case 1:
				cout << "GET" << endl;
				break;
			case 2: 
				cout << "ACK" << endl;
				break;
		}
		
		cout << "Source: " << source << endl;
		cout << "Destination: " << dest << endl;
		
		cout << "Payload: ";
		for (int i = 0; i < paySize; i++)
			cout << payload[i];
		cout << endl;
	}
	
	int seqNum;
	MType mType;
	long int source;
	long int dest;
	char payload[256];
};

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
	int sock, length, n;
	socklen_t fromlen;
	struct sockaddr_in server;
	struct sockaddr_in from;
	vector<Message> messageList;
	Message m, m2, m3;
	char* servBuff[sizeof(Message)];
	
	if (argc < 2)
	{
		fprintf(stderr, "ERROR, no port provided\n");
		exit(0);
	}
	sock=socket(AF_INET, SOCK_DGRAM, 0);
	
	if (sock < 0) 
		error("Opening socket");
		
	length = sizeof(server);
	bzero(&server,length);
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=INADDR_ANY;
	server.sin_port=htons(atoi(argv[1]));
	
	if (bind(sock,(struct sockaddr *)&server,length)<0) 
		error("binding");
		
	fromlen = sizeof(struct sockaddr_in);
	
	while (1)
	{
		n = recvfrom(sock, &m, sizeof(struct Message), 0,(struct sockaddr *)&from,&fromlen);
		
		if (n < 0)
			error("recvfrom");
			
		switch (m.mType)
		{
			case SEND:
				messageList.push_back(m);
				break;
			case GET:
				{
					int j = 0;
					for (int i = 0; i < messageList.size(); i++)
						if (messageList[i].dest == m.source)
							j++;
				
					int numDigits = zeroDelve(j);
				
					for (int i = numDigits - 1; i >= 0; i--)
					{
						m2.payload[i] = numDigits % 10;
						numDigits /= 10;
					}
				
					n = sendto(sock, &m2, sizeof(struct Message), 0, (struct sockaddr*)&from, fromlen);
				
					if (n < 0)
						error("sendto");
					
					for (vector<Message>::iterator it = messageList.begin(); it != messageList.end(); ++it)
					{
						if ((*it).dest == m.source)
						{
							m3 = *it;
							n = sendto(sock, &m3, sizeof(struct Message), 0, (struct sockaddr*)&from, fromlen);
						
							if (n < 0)
								error("sendto");
						
							messageList.erase(it);
							--it;
						}
					}
				}
				break;
		}
		
		m.print();
		
		// Changing M
		m.mType = ACK;
		
		n = sendto(sock, &m, sizeof(struct Message), 0, (struct sockaddr *)&from,fromlen);
		
		if (n < 0) 
			error("sendto");
	}
	
	return 0;
}

