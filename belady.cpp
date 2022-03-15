#include <bits/stdc++.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <thread>

using namespace std;

// cache as list
list<int> cache_queue;
 
// store checkCacheences of key in cache - ISHA doubt
unordered_map<int, int> packet_queue;

// maximum cache size
int csize; 

int size = 6;
int input[6];

/*
 * declare all functions
 */

void showCache();
bool isPresentInCache(int);


int evictPacket (int packetArrivalTime) {
    int res = -1, later_seen = packetArrivalTime;
    for (auto it = cache_queue.begin(); it != cache_queue.end(); it++) {
        int j;
        for (j = packetArrivalTime; j < size; j++) {
            if ((*it) == input[j]) {
                if (j > later_seen) {
                    later_seen = j;
                    res = (*it);
                }
                break;
            }
        }

        if (j == size) {
            return (*it);
        }
    }

    return (res == -1) ? 0 : res;

}

int addPacket(int packetArrivalTime, int x) 
{
    int totalLatency, packetServiceTime, Z;

    if (isPresentInCache(x)) {
	  if (packet_queue.find(x) != packet_queue.end()) {
	      if (packetArrivalTime <= packet_queue[x]) {
			return packet_queue[x] - packetArrivalTime;
		}
		else {
			packet_queue.erase(x);
		}
	  }
	  else
		return 0;
    }
    else {
	  if (cache_queue.size() < csize) {
          	cache_queue.push_front(x);
          	return 0;
	  }
	  else {

	 //Not present in cache, because cache size already exceeded, i. do blocking read, ii. wait if blocking read is already ongoing - insert packet to appropriate packet queue
	 //If no queue present, this is first blocking read
	 if (packet_queue.find(x) == packet_queue.end()) {
		Z = 10;
		totalLatency = Z;
		packetServiceTime = Z + packetArrivalTime;	
		packet_queue[x] = packetServiceTime;
 
		//Eviction should be for element which appears later_seen in input

		int evict = evictPacket (packetArrivalTime);

		if (evict == 0)
			cache_queue.pop_back();
		else
			cache_queue.remove(evict);
		
		cache_queue.push_front(x);
		return totalLatency;
	    }
	}
    }
}

void showCache()
{
    cout << "Showing cache " << endl;
    for (auto it = cache_queue.begin(); it != cache_queue.end(); it++)
        cout << (*it) << " ";
 
    cout << endl;
}

bool isPresentInCache(int x)
{
	for (auto it = cache_queue.begin(); it != cache_queue.end(); it++)
    {
		if (*it == x)
			return 1;
	}
	return 0;
}

int main()
{
    csize = 2;
    int i, idx, total = 0;
    //int input[5000]; // packetArrivalTime is timestamp, value is flow id
    string fname = "trace.csv";
    string line, word;
		 
    /*fstream file (fname, ios::in);
    if(file.is_open())
    {
	  while(getline(file, line))
	  {
	      stringstream str(line);
	      i = 0;	    
	      while(getline(str, word, ';')) {
		   if (i == 0)
		       idx = stoi(word);
	         else
		       input[idx] = stoi(word);	
		   i++;
	      }
         }
     }*/
    
/* 
    for ( i = 0 ; i < 5000; i++)
	  cout << input[i] << " ";     
*/

    input[0] = 1;
    input[1] = 2;
    input[2] = 3;
    input[3] = 1;
    input[4] = 2;
    input[5] = 2;

    cout << "entry " << endl;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    int j = 0;
    while (j < size) {  //TODO change this
	int res = addPacket(j,input[j]); // TODO change this
	total += res;
	j++;
	cout << "j val " << j << endl;
	showCache();
    }

    showCache();

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time execution time = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    cout << "Total latency " << total << endl;
    
    return 0;
}
