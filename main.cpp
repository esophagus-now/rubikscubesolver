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
	
	bool contains(string s) {
		auto it = names.find(s);
		if (it == names.end()) return false;
		else return true;
	}
};

struct perm {
	shared_ptr<nmap> names;
	bool inverted;
	vector<int> mapping;
	string label;
	
	perm() = default;
	
	//Make identity permutation
	perm(shared_ptr<nmap> n) : names(n), inverted(false) {}
	
	//Build permutation from cycle notation in new namespace
	perm(string p) : perm(make_shared<nmap>(),p) {}
	
	//Build permutation from cycle notation in new namespace
	perm(string p, string label) : perm(make_shared<nmap>(),p,label) {}
	
	//Build permutation from cycle notation in another perm's namespace
	perm(perm const& sibling, string p) : perm(sibling.names, p) {}
	
	//Build permutation from cycle notation in a particular namespace
	//(with no label)
	perm(shared_ptr<nmap> n, string p) : perm(n, p, "NL") {}
	
	//Build permutation from cycle notation in a particular namespace
	//with a particular label
	perm(shared_ptr<nmap> n, string p, string label) : 
		names(n), 
		inverted(false),
		label(label) 
	{
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
				if (*++tok == '\0') tok = strtok(NULL, "(" DELIM);
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
	
	//Build permutation from tabular notation in a particular namespace
	//(with no label)
	perm(shared_ptr<nmap> n, vector<int> v) : perm(n, v, "NL") {}
	
	//Build permutation from tabular notation in a particular namespace
	//with a particular label
	perm(shared_ptr<nmap> n, vector<int> v, string label) : 
		names(n), 
		inverted(false), 
		mapping(v),
		label(label) {}
	
	friend int operator^ (int i, perm const& p) {
		if (i < 0 || i >= int(p.mapping.size())) return i;
		
		return p.mapping[i];
	}
	
	perm operator* (perm const& other) const {
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
			for (i = 0; i < int(other.mapping.size()); i++) {
				//BUGFIX: other.mapping may not contain an element for
				//all the outputs of mapping
				newmapping[i] = mapping[i]^other;
			}
			for (;i < int(mapping.size()); i++)
				newmapping[i] = mapping[i];
		}
		
		//cout << "\tnewmapping = " << newmapping << el;
		string newname = label;
		if (inverted) newname += "i";
		newname += "." + other.label;
		if (other.inverted) newname += "i";
		return perm(names, newmapping, newname);
	}
	
	operator bool() const {
		return bool(names);
	}
	
	perm operator-() const {
		perm ret(*this);
		ret.inverted = !inverted;
		for (int i = 0; i < int(mapping.size()); i++)
			ret.mapping[mapping[i]] = i;
		return ret;
	}
	
	perm& invert() {
		inverted = !inverted;
		vector<int> newmapping(mapping.size());
		for (int i = 0; i < int(mapping.size()); i++)
			newmapping[mapping[i]] = i;
		mapping = newmapping;
		return *this;
	}
};

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
	int index = 0;
	for (auto const& i: v) {
		if (i)
			o << delim << (*i.names)[index] << ":\t" << i;
		else
			o << delim << "(" << index << "):\t" << i;
		index++;
	}
	return o << "\n}";
}

vector<perm> orbit(int pos, vector<perm> gens) {
	shared_ptr<nmap> names = gens[0].names;
	
	//ret will contain the Shreier tree
	vector<perm> ret(names->n, perm());
	
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


int main() {
	shared_ptr<nmap> rubiks = make_shared<nmap>();
	perm U(rubiks,"( 1, 3, 8, 6)( 2, 5, 7, 4)( 9,33,25,17)(10,34,26,18)(11,35,27,19)", "U");
	perm L(rubiks,"( 9,11,16,14)(10,13,15,12)( 1,17,41,40)( 4,20,44,37)( 6,22,46,35)", "L");
	perm F(rubiks,"(17,19,24,22)(18,21,23,20)( 6,25,43,16)( 7,28,42,13)( 8,30,41,11)", "F");
	perm R(rubiks,"(25,27,32,30)(26,29,31,28)( 3,38,43,19)( 5,36,45,21)( 8,33,48,24)", "R");
	perm B(rubiks,"(33,35,40,38)(34,37,39,36)( 3, 9,46,32)( 2,12,47,29)( 1,14,48,27)", "B");
	perm D(rubiks,"(41,43,48,46)(42,45,47,44)(14,22,30,38)(15,23,31,39)(16,24,32,40)", "D");
	
	vector<perm> gens = {U,L,F,R,B,D};
	
	auto stree = orbit("1", gens);
	
	cout << stree << el;
	
	int index = 0;
	for (auto const& p : stree) {
		if(p) {
			cout << (*rubiks)[index] << ", ";
		}
		index++;
	}
	cout << el;
	
	perm test = U*F*-U;
	cout << test << el;
	
	for (int i = 0; i < rubiks->n; i++) {
		cout << (*rubiks)[i] << " goes to " << (*rubiks)[i^test] << el;
	}
}
