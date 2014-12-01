#ifndef MESSAGE_H
#define MESSAGE_H

#include <iostream>

using namespace std;

enum MType {SEND = 0, GET = 1, ACK = 2};

struct Message
{	
	Message (int seq = 1, MType t = SEND, long int src = 111111111, long int des = 2222222222, bool cl = true, char p[256] = NULL) : seqNum(seq), mType(t), source(src), dest(des), isClient(cl)
	{
		for (int i = 0; i < 256; i++)
			payload[i] = ' '; 
			
		if (p != NULL)
			for (int i = 0; i < 256; i++)
				payload[i] = p[i];
	}
	
	Message (const Message& other) : seqNum(other.seqNum), mType(other.mType), source(other.source), dest(other.dest), isClient(other.isClient)
	{
		if (other.payload != NULL)
			for(int i = 0; i < 256; i++)
				payload[i] = other.payload[i];
	}
	
	Message& operator=(const Message& other)
	{
		seqNum = other.seqNum;
		mType = other.mType;
		source = other.source;
		dest = other.dest;
		isClient = other.isClient;
	
		for(int i = 0; i < 256; i++)
			payload[i] = other.payload[i];
	}
	
	void print()
	{
		cout << "Sequence Number: " << seqNum << endl;
		cout << "Type: ";
	
		switch(mType)
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
			default:
				break;
		}
	
		cout << "Source: " << source << endl;
		cout << "Destination: " << dest << endl;
		cout << "Is client? " << boolalpha << isClient << endl;
		cout << "Payload Size: 256" << endl;
	
		for (int i = 0; i < 256; i++)
			cout << payload[i];
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
	bool isClient;
};

#endif
