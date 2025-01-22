#include "cache_simulator.h"
#include <iomanip>
using namespace std;

cache *enableCache(string file_name)
{
    ifstream file(file_name);
    string line;
    int i = 1;
    int cache_size;
    int block_size;
    int associativity;
    string write_back_policy;
    string replacement_policy;
    while (getline(file, line))
    {
        switch (i)
        {
        case 1:
            cache_size = stoi(line);
            break;
        case 2:
            block_size = stoi(line);
            break;
        case 3:
            associativity = stoi(line);
            break;
        case 4:
            replacement_policy = line;
            break;
        case 5:
            write_back_policy = line;
            break;
        default:
            cout << "Invalid file format" << endl;
            return NULL;
        }
        i++;
    }
    file.close();
    // if associativity is 0 fully associative cache
    if (associativity == 0)
    {
        associativity = cache_size / block_size;
    }
    cache *newCache = new cache(cache_size, block_size, associativity, write_back_policy, replacement_policy);
    int num_sets = cache_size / (block_size * associativity);
    for (int i = 0; i < num_sets; i++)
    {
        vector<cache_line *> temp;
        for (int j = 0; j < associativity; j++)
        {
            cache_line *newLine = new cache_line(block_size);
            temp.push_back(newLine);
        }
        newCache->table[i] = temp;
    }

    return newCache;
}

void printCacheStatus(cache *newCache)
{
    cout << "Cache Size: " << newCache->cache_size << endl;
    cout << "Block Size: " << newCache->block_size << endl;
    cout << "Associativity: " << newCache->associativity << endl;
    cout << "Replacement Policy: " << newCache->replacement_policy << endl;
    cout << "Write Back Policy: " << newCache->write_back_policy << endl;
}

void invalidateCache(cache *newCache)
{
    for (auto it = newCache->table.begin(); it != newCache->table.end(); it++)
    {
        for (int i = 0; i < it->second.size(); i++)
        {
            it->second[i]->setValid(false);
        }
    }
}

void printCacheStats(cache *newCache)
{
    cout << "D-cache statistics:";
    cout << " Accesses=" << newCache->hits + newCache->misses;
    cout << " ,Hit=" << newCache->hits;
    cout << " ,Miss=" << newCache->misses;
    cout << " ,Hit Rate=" << fixed << setprecision(2) << (((newCache->hits + newCache->misses) != 0) ? ((float)(newCache->hits) / (newCache->hits + newCache->misses)) : 0) << endl;
}

void dumpCache(cache *newCache, string file_name)
{
    ofstream file(file_name);

    for (auto it = newCache->table.begin(); it != newCache->table.end(); it++)
    {
        for (int i = 0; i < it->second.size(); i++)
        {
            if (it->second[i]->valid)
            {
                file << "Set: 0x" << hex << it->first;
                file << " ,Tag: 0x";
                file << hex << stoul(it->second[i]->tag, 0, 2);
                if (it->second[i]->dirty)
                {
                    file << ", Dirty";
                }
                else
                {
                    file << ", Clean";
                }
                file << dec << endl;
            }
        }
    }
    file.close();
}