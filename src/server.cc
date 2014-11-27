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
	
	void clear()
	{
		for (int i = 0; i < 256; i++)
			payload[i] = ' ';
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
	char * servBuff[sizeof(Message)];  
	
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
	
	if (bind(sock,(struct sockaddr *)&server,length) < 0) 
		error("binding");
		
	fromlen = sizeof(struct sockaddr_in);
	
	while (1)
	{
		cout << "Waiting for message" << endl;
		
		m.clear();
		n = recvfrom(sock, &m, sizeof(struct Message), 0,(struct sockaddr *)&from,&fromlen);
		
		if (n < 0)
			error("recvfrom, first in infinite while loop");
		
		switch (m.mType)
		{
			case SEND:
				cout << "Receieved SEND packet from " << m.source << " destined for " << m.dest << endl;
				cout << "Stored incoming message" << endl;
				cout << "Message: " << endl;
				m.print();
				messageList.push_back(m);
				break;
			case GET:
			cout << "GET request from " << m.source << endl;
			
			// Count number of messages destined for the address in the receieved message's source 
			int j = 0;
			
			for (int i = 0; i < messageList.size(); i++)
				if (messageList[i].dest == m.source) 
					j++;
					
			cout << "Found " << j << " messages destined for " << m.source << " in server buffer" << endl; 
			
			// Count the digits in the number of messages to send to know how to write the number of messages to
			// expect into the payload of the outgoing message
			int numDigits = zeroDelve(j);
			
			// Equalize m2 to m
			m2 = m;
			
			// Initialize m2's payload to be blank
			for (int i = 0; i < paySize; i++)
				m2.payload[i] = ' ';
			
			// Write the number of expected packets to m2's payload so it can be sent back to the receiver
			// If there were 150 packets waiting to be sent, m2's payload would look like
			// m[0] = '1', m[1] = '5', m[2] = '0';
			for (int i = numDigits - 1; i >= 0; i--)
			{
				m2.payload[i] = j % 10;
				j /= 10;
			}
			
			// NOTE TO US!!!!!! //////////////////////////////////////////////////////////////
			// If all clients use the same socket, we need to implement packet dropping
			// If all clients use different socket, we need to implement storage for multiple socket IDs
			// and map them somehow
			n = sendto(sock, &m2, sizeof(struct Message), 0,(struct sockaddr *)&from,fromlen);
			
			if (n < 0)
				error("sendto, failed to send number of incoming messages to GET-er");
				
			// Wait 10 milliseconds to make sure they are ready to receive
			usleep(10000); // wait 10000 microseconds
			
			// Iterate through the stored message list looking for messages whose destination matches the
			// source of the incoming GET message, then send that message to intended recipiant, then send
			// ACK back to original sender of the message with appropriate sequence number if the intended
			// recipiant ACKs back
			int k = 0;
			for (vector<Message>::iterator iter = messageList.begin(); iter != messageList.end(); iter++, k++)
			{
				if (messageList[k].dest == m.source)
				{
					cout << "Sending message with sequence number " << messageList[k].seqNum << " to GET-er" << endl;
					
					// Found match, send the message to the GET-er 
					n = sendto(sock, &messageList[k], sizeof(struct Message), 0,(struct sockaddr *)&from,fromlen);
					
					// Error check the send
					if (n  < 0)
						error("sendto, failed to send a stored message to GET-er");
					
					// Wait for response from GET-er
					if (messageList[k].mType != ACK)
					{
						cout << "Waiting for ACK" << endl;
						m3.clear();
						n = recvfrom(sock, &m3, sizeof(struct Message), 0,(struct sockaddr *)&from,&fromlen);
						
						// If the receive failed or the returned message type is not an ACK, error
						if ( (n < 0) || (m3.mType != ACK) )
							error("recvfrom, failed to receive message or message from GET-er was not ACK");
							
						cout << "Got ACK, sending ACK to original sender for sequence number " << messageList[k].seqNum << endl;
						messageList[k] = m3;
					}
					else
					{
						messageList[k] = messageList[messageList.size() - 1];
						messageList.pop_back();
						iter--;
						k--;
					}
					
					// Send ACK to original sender
					//messageList.push_back(m3);
					
					// Error check the send
					//if (n  < 0) error("sendto");
					cout << "ACK sent successfully, removing message from buffer" << endl;
					
					// Don't increment k or the iterator because we have removed an element from the vector
					// and if we increment, we will unintentionally skip an element
				}
			}
			break;
		}
		// Continue the loop FOREVER!!! 
	}
	
	return 0;
}

