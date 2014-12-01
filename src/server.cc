#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <vector>
#include <ctime>
#include <iostream>
#include "Message.h"
#include "ServerInfo.h"

using namespace std; 

// Global stuffs
static int socketIDs[] = {00000,  11111, 22222, 33333, 44444, 55555};
static int serverID = 0;
static string hostNames[6] =
{
	"",
	"127.0.0.1",
	"127.0.0.1",
	"127.0.0.1",
	"127.0.0.1",
	"127.0.0.1"
};

// Function Prototypes
int contains_all(long int, vector<ServerInfo>);
int zeroDelve(long int);
void error(const char* msg);

int main(int argc, char *argv[])
{	
	// Clients
	int sock, length, n;
	int serverRight, serverLeft;
	socklen_t fromlen;
	struct sockaddr_in server;
	struct sockaddr_in from;
	struct sockaddr_in server1, server2;
	struct hostent *hp1, *hp2;
	
	string errorMessage = "Your message has been dropped: destination not valid at this time.";
	
	vector<Message> messageList; 
	vector<ServerInfo> serverInfo;
	
	srand(time(NULL));
	
	serverInfo.reserve(6);
	
	for (int i = 0; i < 6; i++)
	{
		ServerInfo dummyInfo(i);
		cout << dummyInfo.serverID << endl;
		serverInfo.push_back(dummyInfo);
	}
	//serverInfo.push_back(dummyInfo);
	//serverInfo.push_back(dummyInfo);
	serverInfo[1].clients[0] = 1231231234;
	serverInfo[1].clients[1] = 4564564567;
	serverInfo[1].clients[2] = 7897897891;
	serverInfo[1].clients[2] = 1111111111;
	//serverInfo.push_back(dummyInfo);
	serverInfo[2].clients[0] = 2222222222;
	serverInfo[2].clients[1] = 2222222223;
	serverInfo[2].clients[2] = 2222222224;
	//serverInfo.push_back(dummyInfo);
	serverInfo[3].clients[0] = 3333333333;
	serverInfo[3].clients[1] = 3333333334;
	serverInfo[3].clients[2] = 3333333335;
	//serverInfo.push_back(dummyInfo);
	serverInfo[4].clients[0] = 4444444444;
	serverInfo[4].clients[1] = 4444444445;
	serverInfo[4].clients[2] = 4444444446;
	//serverInfo.push_back(dummyInfo);
	serverInfo[5].clients[0] = 5555555555;
	serverInfo[5].clients[1] = 5555555556;
	serverInfo[5].clients[2] = 5555555557;
	
	if (argc < 2)
	{
		fprintf(stderr, "ERROR, no port provided\n");
		exit(0);
	}
	
	serverID = atoi(argv[1]);
	serverRight = serverID+1;
	serverLeft = serverID-1;
	if ( serverRight > 5)
		serverRight = 1;
	
	if (serverLeft < 1)
		serverLeft = 5;
	
	cout << "ServerLeft: " << serverLeft << endl;
	cout << "ServerRight: " << serverRight << endl;
	
	serverInfo[serverID].randomizeVectors(serverLeft, serverRight, serverID);
	
	hp1 = gethostbyname(hostNames[serverRight].c_str());
	hp2 = gethostbyname(hostNames[serverLeft].c_str());
	
	if (hp1 == 0 || hp2 == 0)
		error("Unknown host");
		
	
	server1.sin_family = AF_INET;
	bcopy((char*)hp1->h_addr, (char*)&server1.sin_addr, hp1->h_length);
	
	server2.sin_family = AF_INET;
	bcopy((char*)hp2->h_addr, (char*)&server2.sin_addr, hp2->h_length);
	
	server1.sin_port = htons(socketIDs[serverRight]);
	server2.sin_port = htons(socketIDs[serverLeft]);
	
	sock=socket(AF_INET, SOCK_DGRAM, 0);
	
	if (sock < 0) 
		error("Opening socket");
		
	length = sizeof(server);
	bzero(&server,length);
	
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=INADDR_ANY;
	server.sin_port=htons(socketIDs[serverID]);
	
	if (bind(sock,(struct sockaddr *)&server,length) < 0) 
		error("binding");
	
	fromlen = sizeof(struct sockaddr_in);
	
	sockaddr_in decisionTable[6];
	
	for (int i = 0; i < 6; i++)
		decisionTable[i] = server1;
		
	decisionTable[serverLeft] = server2;
	
	Message m; // Initial received message
	Message m2; // Message used to send
	Message m3; // Used to receive additional subsequent messages
	Message temp;
	ServerInfo dummy(serverID);
	
	char * servBuff[sizeof(Message)];
	  
	cout << "Updating routing tables" << endl;
	
	bool routingCompleted = false; 
	
	/*for(int i = 1; i < 6; i++)
	{
		for(int j = 1; j < 6; j++)
		{
			if (i != j)
			{
				routingCompleted = (serverInfo[i].distanceVector[j] != 0);
				
				if (!(routingCompleted))
					break;
			}
		}
		
		if (!(routingCompleted))
			break;
	}*/
	
	while (!routingCompleted)
	{
		if (serverID == 1)
		{
			cout << "Test1" << endl;
			n = sendto(sock, &(serverInfo[serverID]), sizeof(struct ServerInfo), 0, (struct sockaddr *)&server1, fromlen);
			
			if (n < 0)
			{
				cout << "Error: recvfrom, first in infinite while loop" << endl;
				break;
			}
			cout << "Test2" << endl;
			n = recvfrom(sock, &dummy, sizeof(struct ServerInfo), 0, (struct sockaddr *)&from, &fromlen);
			cout << "Test3" << endl;
			if (n < 0)
			{
				cout << "Error: recvfrom, first in infinite while loop" << endl;
				break;
			}
			
			for (int i = 1; i < 6; i++)
				if( (dummy.distanceVector[i] != 0 ) && (i != serverID) )
					if ((dummy.distanceVector[i] + serverInfo[serverID].distanceVector[serverLeft])  < serverInfo[serverID].distanceVector[i])
						serverInfo[serverID].distanceVector[i] = dummy.distanceVector[i] + serverInfo[serverID].distanceVector[serverLeft];
						
			n = sendto(sock, &(serverInfo[serverID]), sizeof(struct ServerInfo), 0, (struct sockaddr *)&server2, fromlen);
			
			if (n < 0)
			{
				cout << "Error: recvfrom, first in infinite while loop" << endl;
				break;
			}
			
			n = recvfrom(sock, &dummy, sizeof(struct ServerInfo), 0, (struct sockaddr *)&from, &fromlen);
			
			if (n < 0)
			{
				cout << "Error: recvfrom, first in infinite while loop" << endl;
				break;
			}
			
			for (int i = 1; i < 6; i++) 
				if((dummy.distanceVector[i] != 0 ) && (i != serverID))
					if ((dummy.distanceVector[i] + serverInfo[serverID].distanceVector[serverRight])  < serverInfo[serverID].distanceVector[i])
						serverInfo[serverID].distanceVector[i] = dummy.distanceVector[i] + serverInfo[serverID].distanceVector[serverRight];
			cout << "Test4" << endl;
		}
		else
		{
			n = recvfrom(sock, &dummy, sizeof(struct ServerInfo), 0, (struct sockaddr *)&from, &fromlen);
			
			if (n < 0)
			{
				cout << "Error: recvfrom, first in infinite while loop" << endl;
				break;
			}
			
			for (int i = 1; i < 6; i++)
				if((dummy.distanceVector[i] != 0) && (i != serverID))
					if ((dummy.distanceVector[i] + serverInfo[serverID].distanceVector[serverLeft])  < serverInfo[serverID].distanceVector[i])
						serverInfo[serverID].distanceVector[i] = dummy.distanceVector[i] + serverInfo[serverID].distanceVector[serverLeft];
						
			n = sendto(sock, &(serverInfo[serverID]), sizeof(struct ServerInfo), 0, (struct sockaddr *)&server1, fromlen);
			
			if (n < 0)
			{
				cout << "Error: recvfrom, first in infinite while loop" << endl;
				break;
			}
			
			n = recvfrom(sock, &dummy, sizeof(struct ServerInfo), 0, (struct sockaddr *)&from, &fromlen);
			
			if (n < 0)
			{
				cout << "Error: recvfrom, first in infinite while loop" << endl;
				break;
			}
			
			for (int i = 1; i < 6; i++)
				if((dummy.distanceVector[i] != 0 ) && (i != serverID))
					if ((dummy.distanceVector[i] + serverInfo[serverID].distanceVector[serverRight])  < serverInfo[serverID].distanceVector[i])
						serverInfo[serverID].distanceVector[i] = dummy.distanceVector[i] + serverInfo[serverID].distanceVector[serverRight];
						
			n = sendto(sock, &(serverInfo[serverID]), sizeof(struct ServerInfo), 0, (struct sockaddr *)&server2, fromlen);
			
			if (n < 0)
			{
				cout << "Error: recvfrom, first in infinite while loop" << endl;
				break;
			}
		}
		
		for(int i = 1; i < 6; i++)
		{			
			for(int j = 1; j < 6; j++)
			{
				if (i != j)
				{
					routingCompleted = (serverInfo[i].distanceVector[j] != 0);
					
					if (!(routingCompleted))
						break;
				}
			}
			
			if (!(routingCompleted))
				break;
		}
		
		cout << boolalpha << routingCompleted << endl;
		serverInfo[serverID].print();
	}
	
	while (1)
	{
		serverInfo[serverID].print();
		
	  cout << "Waiting for message" << endl;
	  m.clear();
	  // timer goes off
	  
	  n = recvfrom(sock, &m, sizeof(struct Message), 0,(struct sockaddr *)&from,&fromlen);

	  if (m.isClient) {
	 serverInfo[serverID].addClient(m.source);
	 m.isClient = false; 
	  }
	 
	  if (n < 0) {
	  cout << "Error: recvfrom, first in infinite while loop" << endl;
	  break;
	   }

	   
	  switch (m.mType) {
	 case ACK:
		if (serverInfo[serverID].contains(m.dest)){
		   cout << "Receieved ACK packet from " << m.source << " destined for " << m.dest << endl;
		   cout << "Stored incoming message" << endl;
		   cout << "Message: " << endl;
		   m.print();
		   messageList.push_back(m);
		   break;
		} else {
		   int result = contains_all(m.dest, serverInfo);
		   if (result > 0) {
		  cout << "Receieved ACK packet from " << m.source << " destined for " << m.dest << " and we're sending it on..." << endl;
		  
		  n = sendto(sock, &m, sizeof(struct Message), 0,(struct sockaddr *)&server2,fromlen);
		  if (n < 0) {
			 cout << "Error: sendto, failed to forward on message" << endl;
			 break;
		  }
		 
		   } else {
		  long int temp = m.dest;
		  m.dest = m.source;
		  m.source = temp;
		  m.clear();
		  for (int q = 0; q < errorMessage.size(); q++) {
			 m.payload[q] = errorMessage[q];
		  }
		  messageList.push_back(m);
		   }
		  
		   break;
		}

		break; 
	 case SEND:
		if (serverInfo[serverID].contains(m.dest)){
		   cout << "Receieved SEND packet from " << m.source << " destined for " << m.dest << endl;
		   cout << "Stored incoming message" << endl;
		   cout << "Message: " << endl;
		   m.print();
		   messageList.push_back(m);
		   break;
		} else {
		   int result = contains_all(m.dest, serverInfo);
		   if (result > 0) {
		  cout << "Receieved SEND packet from " << m.source << " destined for " << m.dest << " and we're sending it on..." << endl;
		  
		  
		  n = sendto(sock, &m, sizeof(struct Message), 0,(const struct sockaddr *)&server2,fromlen);
		  if (n < 0) {
			 cout << "Error: sendto, failed to forward on message" << endl;
			 break;
		  }
		   } else {
		  long int temp = m.dest;
		  m.dest = m.source;
		  m.source = temp;
		  m.clear();
		  for (int q = 0; q < errorMessage.size(); q++) {
			 m.payload[q] = errorMessage[q];
		  }
		  messageList.push_back(m);
		   }
		   break;
		}
		break;
		
	 case GET:
		 
		cout << "GET request from " << m.source << endl;
		// Count number of messages destined for the address in the receieved message's source 
		int j = 0;
		for (int i = 0; i < messageList.size(); i++) {
		   if (messageList[i].dest == m.source) { 
		  j++;
		   }
		}

		cout << "Found " << j << " messages destined for " << m.source << " in server buffer" << endl; 

		// Count the digits in the number of messages to send to know how to write the number of messages to
		// expect into the payload of the outgoing message
		int numDigits = zeroDelve(j);

		// Equalize m2 to m
		m2 = m;

		// Initialize m2's payload to be blank
		for (int i = 0; i < 256; i++) {
		   m2.payload[i] = ' ';
		}
		 
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

		 if (n < 0) {
		cout << "Error: sendto, failed to send number of incoming messages to GET-er" << endl;
		break;
		 }
		 // Wait 10 milliseconds to make sure they are ready to receive 
		 usleep(10000); // wait 10000 microseconds

		 // Iterate through the stored message list looking for messages whose destination matches the
		 // source of the incoming GET message, then send that message to intended recipiant, then send
		 // ACK back to original sender of the message with appropriate sequence number if the intended
		 // recipiant ACKs back
		 int k = 0;
		 for (vector<Message>::iterator iter = messageList.begin(); iter != messageList.end(); iter++, k++) {

		if (messageList[k].dest == m.source) {

		   cout << "Sending message with sequence number " << messageList[k].seqNum << " to GET-er" << endl;
		   // Found match, send the message to the GET-er 
		   n = sendto(sock, &messageList[k], sizeof(struct Message), 0,(struct sockaddr *)&from,fromlen);

		   // Error check the send
		   if (n  < 0) {
			  cout << "Error: sendto, failed to send a stored message to GET-er" << endl;
			  break;
		   }
		   // Wait for response from GET-er
		   if (messageList[k].mType != ACK)
		   {
			  cout << "Waiting for ACK" << endl;
			  m3.clear();
			  n = recvfrom(sock, &m3, sizeof(struct Message), 0,(struct sockaddr *)&from,&fromlen);
			  
			  // If the receive failed or the returned message type is not an ACK, error
			  if ( (n < 0) || (m3.mType != ACK) ) {
			 cout << "Error: recvfrom, failed to receive message or message from GET-er was not ACK" << endl;
			 break;
			  }
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

		   cout << "ACK sent routingCompletedfully, removing message from buffer" << endl;
		   // Delete the current stored message
		   //messageList.erase(iter);

		   // Don't increment k or the iterator because we have removed an element from the vector
		   // and if we increment, we will unintentionally skip an element
		}
		 }
		 break;
	   }
		   

	   /*
	   m.print();
	   
	   // Changing M
	   m.mType = ACK;
	   
	   n = sendto(sock, &m, sizeof(struct Message), 0,(struct sockaddr *)&from,fromlen);
	   if (n  < 0) error("sendto");*/

	   // Continue the loop FOREVER!!! 
   }
   return 0;
}
 
int contains_all(long int destination, vector<ServerInfo> vec)
{
	for (int k = 1; k < 7; k++)
		for (int i = 0; i < 100; i++)
			if (vec[k].clients[i] == destination)
				return k;
	
	return -1; 
}
	
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

void error(const char *msg)
{
	perror(msg);
	exit(0);
}
