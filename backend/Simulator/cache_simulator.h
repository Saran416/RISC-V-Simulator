#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <climits>
#include <unordered_map>

using namespace std;

class cache_line
{
public:
    bool valid;
    bool dirty;

    vector<unsigned char> data;
    string tag;
    int toa; // most recent time of access

    cache_line(int size)
    {
        valid = false;
        dirty = false;
        toa = 0;
        this->data.resize(size);
        tag = "";
    }

    void setTag(string tag)
    {
        this->tag = tag;
    }

    void setValid(bool valid)
    {
        this->valid = valid;
    }
    
    void setDirty(bool dirty)
    {
        this->dirty = dirty;
    }
};

class cache
{
public:
    int hits;
    int misses;
    unordered_map<int, vector<cache_line *> > table;
    int cache_size;
    int block_size;
    int associativity;
    string write_back_policy;
    string replacement_policy;

    cache(int cache_size, int block_size, int associativity, string write_back_policy, string replacement_policy)
    {
        this->cache_size = cache_size;
        this->block_size = block_size;
        this->associativity = associativity;
        this->write_back_policy = write_back_policy;
        this->replacement_policy = replacement_policy;
        this->hits = 0;
        this->misses = 0;
    }
};

cache *enableCache(string file_name);

void printCacheStatus(cache *newCache);

void invalidateCache(cache *newCache);

void printCacheStats(cache *newCache);

void dumpCache(cache *newCache, string file_name);