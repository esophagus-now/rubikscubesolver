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
	vector<int> mapping;
	bool inverted;
	
	perm() = default;
	
	//Make identity permutation
	perm(shared_ptr<nmap> n) : names(n), cycles{0}, inverted(false) {}
	
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
		
		mapping(vector<int>(names->n);
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
	
	perm(shared_ptr<nmap> n, vector<int> v) : names(n), inverted(false), mapping(v) {
		//This commented code will eventually get used for pretty-printing
		/*
		//cout << "\t" << v << el;
		cycles.push_back(0);
		
		vector<int> marked(names->n, 0);
		
		int pos = 0;
		int in_cycle = 0;
		while (pos < names->n) {
			if (v[pos] == pos) {
				marked[pos] = 1;
				in_cycle = 0;
				pos++; //Skip singletons
			} else if (marked[pos]) {
				if (in_cycle) cycles.push_back(data.size()); 
				in_cycle = 0;
				pos++; //Skip marked elements
			} else {
				marked[pos] = 1;
				data.push_back(pos);
				pos = v[pos];
				in_cycle = 1;
			}
		}
		* */
	}
	
	perm operator* (perm const& other) {
		if (names != other.names) return *this;
		
		vector<int> cur(names->n);
		iota(begin(cur), end(cur), 0);
		
		if (other.inverted == false) {
			for (int i = int(other.cycles.size()) - 1; i > 0; i--) {
				//other.cycles[i]-1 is rightmost element in this cycle
				int j = other.cycles[i]-1;
				//Cache the location of the "right parenthesis" (see TAOCP)
				int hole_index = other.data[j];
				//tmp is the current value at that position
				int tmp = cur[other.data[j]];
				for (j--; j >= other.cycles[i-1]; j--) swap(cur[other.data[j]], tmp);
				cur[hole_index] = tmp;
			}
		} else {
			for (int i = int(other.cycles.size()) - 1; i > 0; i--) {
				//other.cycles[i-1] is leftmost element in this cycle
				int j = other.cycles[i-1];
				//Cache the position of the "leftt parenthesis" (see TAOCP)
				int hole_index = other.data[j];
				//tmp is the current value at that position
				int tmp = cur[other.data[j]];
				
				for (; j < other.cycles[i]; j++) swap(cur[other.data[j]], tmp);
				cur[hole_index] = tmp;
			}
		}
		
		if (inverted == false) {
			for (int i = int(cycles.size()) - 1; i > 0; i--) {
				int j = cycles[i]-1;
				int tmp = cur[data[j]];
				int hole_index = data[j];
				for (j--; j >= cycles[i-1]; j--) swap(cur[data[j]], tmp);
				cur[hole_index] = tmp;
			}
		} else {
			for (int i = int(cycles.size()) - 1; i > 0; i--) {
				//other.cycles[i-1] is leftmost element in this cycle
				int j = cycles[i-1];
				//Cache the position of the "leftt parenthesis" (see TAOCP)
				int hole_index = data[j];
				//tmp is the current value at that position
				int tmp = cur[data[j]];
				
				for (; j < cycles[i]; j++) swap(cur[data[j]], tmp);
				cur[hole_index] = tmp;
			}
		}
		
		return perm(names, cur);
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
	if (i < 0 || i >= p.names->n) return i;
	
	auto it = find(begin(p.data), end(p.data), i);
	if (it == end(p.data)) return i;
	int index = it - begin(p.data);
	index++;
	auto cyc_it = find(begin(p.cycles), end(p.cycles), index);
	if (cyc_it != end(p.cycles)) {
		//Need to wrap back
		index = *(--cyc_it);
	}
	return p.data[index];
}

ostream& operator<< (ostream& o, perm const& p) {
	//cout << "\t" << p.data << "\n\t" << p.cycles << el;
	
	if(p.cycles.size() == 1) {
		return o << "()";
	}
	
	for (unsigned i = 0; i < p.cycles.size() - 1; i++) {
		auto delim = "(";
		for (int j = p.cycles[i]; j < p.cycles[i+1]; j++) {
			o << delim << (*p.names)[p.data[j]];
			delim = ",";
		}
		o << ")";
	}
	
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
	
	cout << "Hello world" << el;
	for (int i = 1; i < 9; i++) cout << (i^p3) << el;
}