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
 
/* map containing packet's service time 
 * during cache miss and used for computing
 * aggregate delay for delayed hits
 */
unordered_map<int, int> packet_queue;

// cache size
int csize; 

// Trace containing 5000 requests
int size = 5000;
int input[5000];
int defFetchTime = 0, fetchTime = 3, hitrate = 0, missrate = 0;

void showCache();
bool isPresentInCache(int);

/* This function adds an object in the cache, 
 * tracks hit rate, miss rate, total latency
 * to add the object. In case of cache miss, 
 * it evicts an object from the cache based 
 * on relevant caching algorithm. Additionaly,
 * the latency caused due to delayed hits is 
 * tracked.
 */

int addPacket(int packetArrivalTime, int x) 
{
    /* Z is computed with object fetch time to 
     * request inter-arrival time
     */
    int totalLatency, packetServiceTime, Z;

    if (isPresentInCache(x)) {
	  // As object present in cache, hit rate is incremented
	  hitrate++;
	    
	  /* If there exists a queue for the object due to previous
	   * cache miss, need to take into account the aggregate 
	   * delay caused for the arrival of burst of requests for 
	   * same object till the time the cache miss is resolved
	   */
	  if (packet_queue.find(x) != packet_queue.end()) {
	      if (packetArrivalTime <= packet_queue[x]) {
			return packet_queue[x] - packetArrivalTime;
		}
		else {
			packet_queue.erase(x);
			return defFetchTime;
		}
	  }
	  else {
		// When the object is in cache, consider zero latency 
		return defFetchTime;
	  }
    }
    else {
	 // As object not present in cache, miss rate is incremented
	 missrate++;
	 if (cache_queue.size() < csize) {
		// If the cache can accomodate new object, insert
          	cache_queue.push_front(x);
          	return 0;
    	 }
	 else {
		// If the cache is already full, object eviction is done
		Z = fetchTime;
		totalLatency = Z;
		packetServiceTime = Z + packetArrivalTime;	
		packet_queue[x] = packetServiceTime;
 
		// Eviction should be based on least cost delay
		// Calculate the request inter-arrival time for the object 
		int cnt = 0;
		for (int i = packetArrivalTime; i < packetArrivalTime + fetchTime && i < size; i++) {  
		    if (x == input[i]) {
			   cnt++;
		    }
		}
		 
		// Calculate Z factor to be used for aggregate delay calculation
		if (cnt != 0) {
			Z = fetchTime*cnt;
		}
		else {
			Z = fetchTime;
		}

		// This map tracks the delay cost for the cache objects
		map<int,int> leastCostDelayMap;
		for (auto it = cache_queue.begin(); it != cache_queue.end(); it++) {
			for (int i = packetArrivalTime; i < packetArrivalTime +  Z && i < size; i++) {
				if (i == packetArrivalTime)
					leastCostDelayMap[(*it)] = 0;			
				if (input[i] == (*it)) {
					leastCostDelayMap[(*it)] += 1;
				}
			}
		}

		// Compute the aggregated delay for cache objects
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
		 
		/*
		// Find TTNA of all elements in cache
		map <int,int> ttna;
		for (auto it = cache_queue.begin(); it != cache_queue.end(); it++) {
			ttna[(*it)] = -1;
			for (int i = packetArrivalTime; i < packetArrivalTime +  Z && i < size; i++) {
				if (input[i] == (*it)) {
					ttna[(*it)] = i-packetArrivalTime;
					break;
				}
			}
		}

		// Compute agg_delay/TTNA for cache objects
		for (auto it = leastCostDelayMap.begin(); it != leastCostDelayMap.end(); it++) {
			int ttna_val = ttna[(*it).first];
			leastCostDelayMap[(*it).first] = (*it).second/ttna_val; 
		}
		*/
		 
		// Eviction should be based on least cost agg delay/TTNA value within window Z for Belatedly
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

void showCache()
{
    for (auto it = cache_queue.begin(); it != cache_queue.end(); it++)
        cout << (*it) << " ";
 
    cout << endl;
}

// Return true if object is already present in cache, else false
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
    csize = 16; // cache size
    int i, idx, total = 0;
    
    // parser to encode packet arrival time and flow id from trace
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

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    int j = 0;
    while (j < size) {  
	int res = addPacket(j,input[j]);
	total += res;
	j++;
    }

    showCache();

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "BELADY AGG DELAY Time execution time = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    cout << "Total latency " << total << " hit_rate " << hitrate/5000 << " miss_rate " << missrate/5000 << endl;
    
    return 0;
}
