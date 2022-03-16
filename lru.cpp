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

/*
 * declare all functions
 */
int size = 5000, hitrate = 0, fetchTime = 3, missrate = 0;

bool isPresentInCache(int);

/*
void aggDelay()
{

}
*/

int addPacket(int packetArrivalTime, int x) 
{
    int totalLatency, packetServiceTime, timeToFetch;

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
	 	if (packet_queue.find(x) == packet_queue.end()) {
		timeToFetch = fetchTime;
		totalLatency = timeToFetch;
		packetServiceTime = timeToFetch + packetArrivalTime;	
		packet_queue[x] = packetServiceTime;

		//insert in cache
		cache_queue.pop_back();
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
    int input[5000]; // index is timestamp, value is flow id
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
    
    //orig input
    //int input1[10] = {1,1,2,3};

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    int j = 0;
    while (j < size) {  //TODO change this
	int res = addPacket(j,input[j]); // TODO change this
	total += res;
	j++;
    }

    showCache();

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "LRU Time execution time = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    cout << "Total latency " << total << " hit_rate " << hitrate << " miss_rate " << missrate << endl;
    
    return 0;
}
