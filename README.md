# CS 740 - Caching with Delayed Hits

Caching has grown to be very important especially for systems which are prone to delays. We have come to understand that cache requests lead to either cache hits or cache misses. However, Nirav et. Al. (in Caching with delayed hits) have described another and most important result known as delayed hits. During the time when cache miss occurs, and the latency to fetch the same object from the backing store is more compared to the inter-arrival time request for the object, it results in delayed hits. These delayed hits are commonly found during high throughput requests, especially when many requests are piled up in the wait queue for the same missing object for which the cache miss is getting resolved leading to increased latency. This is an interesting observation and has been overlooked in many traditional caching research works. I have designed simple prototype of latency-minimizing caching algorithms that are aware of delayed hits.

Requirement : trace.csv input file containing 5000 requests 

To compile :

g++ {filename}.cpp

To execute :

./a.out 

Sample:

isha@node0:~/740_delayed_hits$ g++ belatedly.cpp
  
isha@node0:~/740_delayed_hits$ ./a.out


BELATEDLY Time execution time = 107982[µs]

Total latency 5350 hit_rate 0.2918 miss_rate 0.7082
