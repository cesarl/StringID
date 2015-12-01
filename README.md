# StringID
String to ID lib and offline generator


#### TODO

- [x] Map serialization (human readable, binary - ~~maybe compress it in lz4~~)
- [x] String buffer pool serialization (binary only)
- [ ] Function to clear map and string buffer pool for manager
- [x] Multi-threaded manager
- [ ] Large set of test
- [ ] Cache last modification dates (still buggy)

##### Maybe

- [ ] Lockfree hash table in place of std::unordered_map and std::mutex. ( see http://preshing.com/20130605/the-worlds-simplest-lock-free-hash-table/ )
