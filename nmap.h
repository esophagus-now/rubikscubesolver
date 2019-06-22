#ifndef NMAP_H
#define NMAP_H 1

#include <map>
#include <vector>
#include <string>

struct nmap {
	std::map<std::string, int> names;
	int n;
	std::vector<std::string const *> rev_lookup;
	
	using iterator = std::map<std::string, int>::iterator;
	
	nmap() : n(0) {}
	
	//Inserts the string s into the namespace, possibly assigning a new
	//number. Returns the internal number for that string
	int insert(std::string s) {
		bool inserted;
		iterator it;
		tie(it, inserted) = names.insert({s, n});
		if (inserted) {
			n++; //If new element, increase id
			rev_lookup.push_back(&(it->first));
		}
		return it->second;
	}
	
	std::string const& operator[] (int x) {return *rev_lookup[x];}
	int& operator[] (std::string const& x) {return names[x];}
	
	bool contains(std::string s) {
		auto it = names.find(s);
		if (it == names.end()) return false;
		else return true;
	}
};

#endif

