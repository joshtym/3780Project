#ifndef SERVERINFO_H
#define SERVERINFO_H

struct ServerInfo
{
	ServerInfo(int givenServerID = 0) : serverID(givenServerID)
	{
		for (int i = 0; i < 6; i++)
		{
			if (i == 0 || i == serverID)
				distanceVector[i] = 0;
			else
				distanceVector[i] = 90;
		}
			
		for (int i = 0; i < 100; i++)
			clients[i] = 0;
	}
	
	void addClient(long int clientID)
	{
		int i = 0;
		
		while (clients[i] != 0 && i < 99)
		{
			if (clients[i] == clientID)
				break;
				
			i++;
		}
		
		clients[i] = clientID;
	}
	
	bool contains(long int destination)
	{
		for (int i = 0; i < 100; i++)
			if (clients[i] == destination)
				return true;
				
		return false;
	}
	
	void randomizeVectors(int serverLeft, int serverRight, int serverID)
	{
		distanceVector[serverID] = 0;
		distanceVector[serverLeft] = rand() % 10 + 1;
		distanceVector[serverRight] = rand() % 10 + 1;
		throughWhichRouter[serverLeft] = serverLeft;
		throughWhichRouter[serverRight] = serverRight;
	}
	
	void print()
	{
		cout << "Server ID is: " << serverID << endl;
		
		for (int i = 1; i < 6; i++)
			if (i != serverID)
				cout << "Distance to server with ID " << i << " is " << distanceVector[i] << " and is through Server # " << throughWhichRouter[i] << endl;
		
		cout << "This server currently hosts for the following clients: " << endl;
		for (int i = 0; i < 100; i++)
			if (clients[i] != 0)
				cout << clients[i] << endl;
		
		cout << endl;
	}
	
	int distanceVector[6];
	int throughWhichRouter[6];
	long int clients[100];
	int serverID;
};

#endif
