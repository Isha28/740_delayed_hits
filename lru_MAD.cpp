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

int size = 5000;
int input[5000];
int hitrate = 0, missrate = 0, fetchTime = 3;

/*
 * declare all functions
 */

void showCache();
bool isPresentInCache(int);

int addPacket(int packetArrivalTime, int x) 
{
    int totalLatency, packetServiceTime, Z;

    if (isPresentInCache(x)) {
	  hitrate++;
	  if (packet_queue.find(x) != packet_queue.end()) {
	        if (packetArrivalTime <= packet_queue[x]) {
			return packet_queue[x] - packetArrivalTime;
		}
		else {
			cache_queue.remove(x);    
                        cache_queue.push_front(x);
			packet_queue.erase(x);
			return 0;
		}
	  }
	  else {
		cache_queue.remove(x);
                cache_queue.push_front(x);
		return 0;
	  }
    }
    else {
	missrate++;
	if (cache_queue.size() != csize) {
          	cache_queue.push_front(x);
          	return 0;
    	}
	else {
	 //Not present in cache, because cache size already exceeded, i. do blocking read, ii. wait if blocking read is already ongoing - insert packet to appropriate packet queue
	 //If no queue present, this is first blocking read
	 if (packet_queue.find(x) == packet_queue.end()) {
		Z = fetchTime;
		totalLatency = Z;
		packetServiceTime = Z + packetArrivalTime;	
		packet_queue[x] = packetServiceTime;
 
		//Eviction should be based on least delay cost
		//check if this element - agg delay has lower cost 
		int cnt = 0;
		for (int i = packetArrivalTime; i < packetArrivalTime + fetchTime && i < size; i++) {  //input array size is 10
		    if (x == input[i]) {
			   cnt++;
		    }
		}	
		if (cnt != 0) {
			Z = fetchTime*cnt;
		}
		else {
			Z = fetchTime;
		}

		map<int,int> leastCostDelayMap;
		for (auto it = cache_queue.begin(); it != cache_queue.end(); it++) {
			for (int i = packetArrivalTime; i > packetArrivalTime - Z && i >= 0; i--) {
				if (i == packetArrivalTime)
					leastCostDelayMap[(*it)] = 0;			
				if (input[i] == (*it)) {
					leastCostDelayMap[(*it)] += 1;
				}
			}
		}

		for (auto it = leastCostDelayMap.begin(); it != leastCostDelayMap.end(); it++) {
           		if ((*it).second == 0)
				continue;
			int prev = (*it).second;
                	(*it).second = fetchTime;
                	int add = fetchTime-1;
                	for (int i = prev; i > 1; i--) {
                        	(*it).second += add;
                        	add -= 1;
                	}
        	}

		int leastFreq = INT_MAX, leastCostElement = 0;
		for (auto it = leastCostDelayMap.begin(); it != leastCostDelayMap.end(); it++) {
			if ((*it).second < leastFreq) {
				leastFreq = (*it).second;
				leastCostElement = (*it).first;
			}

		}

		if (leastCostElement == 0)
			cache_queue.pop_back();
		else
			cache_queue.remove(leastCostElement);
		
		cache_queue.push_front(x);
		return totalLatency;
		}
	    } 
	}
}

void showCache()
{
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
    csize = 4;
    int i, idx, total = 0;
    //int input[5000]; // index is timestamp, value is flow id
    string fname = "trace.csv";
    string line, word;
		 
    fstream file (fname, ios::in);
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
     }

    /* orig input
    input[0] = 1;
    input[1] = 1;
    input[2] = 2;
    input[3] = 3;
    //input[4] = 2;
    //input[5] = 2;
    */

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    int j = 0;
    while (j < size) {  //TODO change this
	int res = addPacket(j,input[j]); // TODO change this
	total += res;
	j++;
    }

    showCache();

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "LRU AGG DELAY Time execution time = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    cout << "Total latency " << total << " hit_rate " << hitrate << " miss_rate " << missrate << endl;
    
    return 0;
}
