 
#ifndef UTIL_SORTED_SET_H_
#define UTIL_SORTED_SET_H_

#include <inttypes.h>
#include <string>
#include <map>
#include <set>

class SortedSet
{
public:
	bool empty() const{
		return size() == 0;
	}
	int size() const;
	int add(const std::string &key, int64_t score);
	// 0: not found, 1: found and deleted
	int del(const std::string &key);
	// the first item is copied into key if SortedSet not empty
	int front(std::string *key, int64_t *score=NULL) const;
	int back(std::string *key, int64_t *score=NULL) const;
	int64_t max_score() const;
	int pop_front();
	int pop_back();
	
	 

private:
	struct Item
	{
		std::string key;
		int64_t score;
		
		bool operator<(const Item& b) const{
			return this->score < b.score
				|| (this->score == b.score && this->key < b.key);
		}
	};
	
	std::map<std::string, std::set<Item>::iterator> existed;
	std::set<Item> sorted_set;
};


 
 

#endif
