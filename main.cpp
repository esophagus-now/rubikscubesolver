#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <utility> //pair
#include <memory> //shared_ptr
#include <cstring> //strtok
#include <numeric> //iota
#include <algorithm>
#include <set>
#include <queue>

#define DELIM ",) \t"

using namespace std;

ostream& el(ostream& o) {
	return o << "\n";
}

template <typename T>
ostream& operator<< (ostream& o, vector<T> const& v) {
	o << "[";
	auto delim = "";
	for (auto const& i: v) {
		o << delim << i;
		delim = ",";
	}
	return o << "]";
}

template <typename T, typename U>
ostream& operator<< (ostream& o, map<T,U> const& m) {
	o << "[";
	auto delim = "";
	for (auto const& i: m) {
		o << delim << i.first << "=" << i.second;
		delim = "|";
	}
	return o << "]";
}

struct nmap {
	map<string, int> names;
	int n;
	vector<string const *> rev_lookup;
	
	using iterator = map<string, int>::iterator;
	
	nmap() : n(0) {}
	
	//Inserts the string s into the namespace, possibly assigning a new
	//number. Returns the internal number for that string
	int insert(string s) {
		bool inserted;
		iterator it;
		tie(it, inserted) = names.insert({s, n});
		if (inserted) {
			n++; //If new element, increase id
			rev_lookup.push_back(&(it->first));
		}
		return it->second;
	}
	
	string const& operator[] (int x) {return *rev_lookup[x];}
	int& operator[] (string const& x) {return names[x];}
};

struct perm {
	shared_ptr<nmap> names;
	bool inverted;
	vector<int> mapping;
	
	perm() = default;
	
	//Make identity permutation
	perm(shared_ptr<nmap> n) : names(n), inverted(false) {}
	
	perm(string p) : perm(make_shared<nmap>(),p) {}
	
	perm(perm const& sibling, string p) : perm(sibling.names, p) {}
	
	perm(shared_ptr<nmap> n, string p) : names(n), inverted(false) {
		//This is to copy the strings, since strtok modifies input
		vector<char> pcopy(begin(p), end(p));
		pcopy.push_back('\0');
		
		vector<int> data;
		vector<int> cycles;
		
		char *tok = strtok(pcopy.data(), DELIM);
		int pos = 0;
		
		while(tok) {
			if (*tok == '(') {
				cycles.push_back(pos);
				tok++;
			}
			int val = names->insert(tok);
			data.push_back(val);
			tok = strtok(NULL, DELIM);
			pos++;
		}
		cycles.push_back(pos); //Pushes the size of data to the end of cycles
		//in order to simplify iteration
		
		mapping = vector<int>(names->n);
		iota(begin(mapping), end(mapping), 0);
		
		for (int i = int(cycles.size()) - 1; i > 0; i--) {
			//cycles[i]-1 is rightmost element in this cycle
			int j = cycles[i]-1;
			//Cache the location of the "right parenthesis" (see TAOCP)
			int hole_index = data[j];
			//tmp is the current value at that position
			int tmp = mapping[data[j]];
			for (j--; j >= cycles[i-1]; j--) swap(mapping[data[j]], tmp);
			mapping[hole_index] = tmp;
		}
	}
	
	perm(shared_ptr<nmap> n, vector<int> v) : names(n), inverted(false), mapping(v) {}
	
	perm operator* (perm const& other) {
		if (names != other.names) return *this;
		
		int n_larger = max(mapping.size(), other.mapping.size());
		vector<int> newmapping(n_larger);
		//iota(begin(newtab), end(newtab), 0);
		
		if (mapping.size() < other.mapping.size()) {	
			int i;	
			for (i = 0; i < int(mapping.size()); i++)
				newmapping[i] = other.mapping[mapping[i]];
			for (;i < int(other.mapping.size()); i++)
				newmapping[i] = other.mapping[i];
		} else {	
			int i;
			for (i = 0; i < int(other.mapping.size()); i++)
				newmapping[i] = other.mapping[mapping[i]];
			for (;i < int(mapping.size()); i++)
				newmapping[i] = mapping[i];
		}
		
		return perm(names, newmapping);
	}
	
	operator bool() {
		return bool(names);
	}
	
	perm operator-() {
		perm ret(*this);
		ret.inverted = !inverted;
		return ret;
	}
	
	perm& invert() {
		inverted = !inverted;
		return *this;
	}
};

int operator^ (int i, perm const& p) {
	if (i < 0 || i >= int(p.mapping.size())) return i;
	
	return p.mapping[i];
}

ostream& operator<< (ostream& o, perm const& p) {
	//cout << p.mapping << el;
	
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


vector<perm> orbit(int pos, vector<perm> gens) {
	shared_ptr<nmap> names = gens[0].names;
	vector<perm> ret(names->n, perm());
	queue<int> frontier;
	frontier.push(pos);
	
	while(!frontier.empty()) {
		int cur = frontier.front();
		frontier.pop();		
	}
	
	return ret;
}


int main() {
	string perm1 = "(A,C,8,F)(2,5,CHARLIE,4)";
	string perm2 = "(C,5,8)";
	perm p1(perm1);
	perm p2(p1, perm2);
	
	cout << p1 << el;
	cout << p2 << el;
	
	perm p3 = p1*-p2;
	cout << "p3 = " << p3 << el;
	p3 = p3*p2;
	cout << p3 << el;
	p3 = p3*-p3;
	cout << p3 << el;
	
	perm p3tmp = p3;
	for (int i = 0; i < 14; i++) p3 = p3*p3tmp;
	
	cout << "Hello world" << el;
	cout << p3 << el;
	for (int i = 1; i < 9; i++) cout << (i^p3) << el;
}
