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
#include <cmath>

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

int main(int argc, char **argv)
{
	int sock, n;
	unsigned int length;
	struct sockaddr_in server, from;
	struct hostent *hp;
	char buffer[256];
	int userChoice = 0;
	long int clientNum;
	long int destNum;
	int seqNum = 0;
	Message m, m2, m3;
	
	if (argc !=3)
	{
		cout << "Usage: server port" << endl;
		exit(1);
	}
	
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	
	if (sock < 0)
		error("socket");
		
	server.sin_family = AF_INET;
	hp = gethostbyname(argv[1]);
	if (hp == 0)
		error("Unknown host");
		
	bcopy((char*)hp->h_addr, (char*)&server.sin_addr, hp->h_length);
	server.sin_port = htons(atoi(argv[2]));
	length = sizeof(struct sockaddr_in);
	printf("Please enter the message: ");
	bzero(buffer, 256);
	fgets(buffer, 255, stdin);
	
	cout << "Please enter your 10 digit client number: ";
	cin >> clientNum;

	while (1)
	{
		cout << "What would you like to do ('0' for SEND, '1' for GET, and '2' for quit): ";
		cin >> userChoice;
		
		string message;
		
		if (userChoice == 0)
		{
			cout << "Please enter the destination address: ";
			cin >> destNum;
			cin.ignore();
			
			cout << "What message would you like to send: ";
			getline(cin, message);
			
			for (int i = 0; i < message.length(); i++)
				m.payload[i] = message[i];
				
			m.seqNum = seqNum;
			m.mType = SEND;
			m.source = clientNum;
			m.dest = destNum;
			
			n = sendto(sock, &m, sizeof(struct Message), 0, (const struct sockaddr*)&server, length);
			
			if (n < 0) 
				error("Sendto");
				
			seqNum++;
		}
		else if(userChoice == 1)
		{
			m.mType = GET;
			m.seqNum = seqNum;
			m.source = clientNum;
			m.dest = 1111111111;
			
			n = sendto(sock, &m, sizeof(struct Message), 0, (const struct sockaddr*)&server, length);
			
			if (n < 0)
				error("Sendto");
				
			n = recvfrom(sock, &m2, sizeof(struct Message),0,(struct sockaddr *)&from, &length);
			
			if (n < 0)
				error("recvfrom");
			
			int i, j, k = 0;
			int numIncoming = 0;
			char result[256];
			while (m2.payload[i] != ' ')
			{
				result[i] = m2.payload[i];
				i++;
			}
			
			k = i;
			
			while (j < k)
			{
				numIncoming += (int)result[j] * pow(10, i-1);
				i--;
				j++;
			}
			
			for (int i = 0; i < numIncoming; i++)
			{
				n = recvfrom(sock, &m3, sizeof(struct Message),0,(struct sockaddr *)&from, &length);
				
				if (n < 0)
					error("recvfrom");
					
				m3.print();
				m3.mType = ACK;
				
				n = sendto(sock, &m3, sizeof(struct Message), 0, (struct sockaddr *)&server, length);
				
				if (n < 0)
					error("sendto");
			}
		}
		else if (userChoice == 2)
			break;
		else
			cout << "Invalid entry. Please select 0, 1, or 2: ";
	}

	close(sock);
	return 0;
}
