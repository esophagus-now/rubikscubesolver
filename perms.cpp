#include "perms.h"
#include <queue>
#include <iostream>

using namespace std;

vector<perm> orbit(int pos, vector<perm> gens) {
	shared_ptr<nmap> names = gens[0].names;
	
	//ret will contain the Shreier tree
	vector<perm> ret(names->n, perm());
	ret[0] = perm(gens[0].names); //Create identity
	
	queue<int> frontier;
	frontier.push(pos);
	
	//Perform BFS
	while(!frontier.empty()) {
		int cur = frontier.front();
		frontier.pop();
		for (auto const& p : gens) {
			int dest = cur^-p;
			if (!ret[dest]) {
				//We discovered a new element in the orbit
				ret[dest] = p;
				frontier.push(dest);
			}
			dest = cur^p;
			if (!ret[dest]) {
				//We discovered a new element in the orbit
				ret[dest] = -p;
				frontier.push(dest);
			}
		}
	}
	
	return ret;
}

vector<perm> orbit(string label, vector<perm> gens) {
	shared_ptr<nmap> names = gens[0].names;
	
	//ret will contain the Shreier tree
	vector<perm> ret(names->n, perm());
	
	if (!names->contains(label)) return ret; //Orbit is empty
	
	int pos = (*names)[label];
	
	return orbit(pos, gens);
}

ostream& operator<< (ostream& o, perm const& p) {
	//cout << p.mapping << el;
	
	if (p.label != "NL") {
		o << "[" << p.label;
		if (p.inverted) o << "i"; 
		o << "]<=>";
	}
	
	vector<int> marked(p.mapping.size(), 0);
	auto delim = "(";
	
	bool wrote_something =  false;
	int pos = 0;
	int in_cycle = 0;
	while (pos < int(p.mapping.size())) {
		if ((pos^p) == pos) {
			marked[pos] = 1;
			in_cycle = 0;
			pos++; //Skip singletons
		} else if (marked[pos]) {
			if (in_cycle) o << ")";
			in_cycle = 0;
			pos++; //Skip marked elements
		} else {
			marked[pos] = 1;
			//data.push_back(pos);
			if (in_cycle == 0) delim = "(";
			o << delim << (*p.names)[pos];
			wrote_something = true;
			delim = ",";
			pos = pos^p;
			in_cycle = 1;
		}
	}
	
	if (!wrote_something) o << "()";
	
	return o;
}

template <>
ostream& operator<< <perm> (ostream& o, vector<perm> const& v) {
	o << "{";
	auto delim = "\n\t";
	//int index = 0;
	for (auto const& i: v) {
		/*
		if (i) {
			nmap tmp = *i.names;
			o << delim << tmp[index];
			o << ":\t" << i;
		} else
			o << delim << "(" << index << "):\t" << i;
		index++;
		* */
		o << delim << i;
	}
	return o << "\n}";
}
