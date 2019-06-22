#ifndef PERMS_H
#define PERMS_H 1

#include "nmap.h"
#include <memory> //shared_ptr
#include <utility> //make_shared
#include <vector>
#include <string>
#include <cstring> //strtok
#include <algorithm> //max
#include <numeric> //iota
#include <iostream>
#include "marco.h"


#define DELIM ",) \t"

struct perm {
	std::shared_ptr<nmap> names;
	bool inverted;
	std::vector<int> mapping;
	std::string label;
	
	perm() = default;
	
	//Make identity permutation
	perm(std::shared_ptr<nmap> n) : names(n), inverted(false) {}
	
	//Build permutation from cycle notation in new namespace
	perm(std::string p) : perm(std::make_shared<nmap>(),p) {}
	
	//Build permutation from cycle notation in new namespace
	perm(std::string p, std::string label) : perm(std::make_shared<nmap>(),p,label) {}
	
	//Build permutation from cycle notation in another perm's namespace
	perm(perm const& sibling, std::string p) : perm(sibling.names, p) {}
	
	//Build permutation from cycle notation in a particular namespace
	//(with no label)
	perm(std::shared_ptr<nmap> n, std::string p) : perm(n, p, "NL") {}
	
	//Build permutation from cycle notation in a particular namespace
	//with a particular label
	perm(std::shared_ptr<nmap> n, std::string p, std::string label) : 
		names(n), 
		inverted(false),
		label(label) 
	{
		//This is to copy the strings, since strtok modifies input
		std::vector<char> pcopy(begin(p), end(p));
		pcopy.push_back('\0');
		
		std::vector<int> data;
		std::vector<int> cycles;
		
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
		
		mapping = std::vector<int>(names->n);
		std::iota(begin(mapping), end(mapping), 0);
		
		for (int i = int(cycles.size()) - 1; i > 0; i--) {
			//cycles[i]-1 is rightmost element in this cycle
			int j = cycles[i]-1;
			//Cache the location of the "right parenthesis" (see TAOCP)
			int hole_index = data[j];
			//tmp is the current value at that position
			int tmp = mapping[data[j]];
			for (j--; j >= cycles[i-1]; j--) std::swap(mapping[data[j]], tmp);
			mapping[hole_index] = tmp;
		}
	}
	
	//Build permutation from tabular notation in a particular namespace
	//(with no label)
	perm(std::shared_ptr<nmap> n, std::vector<int> v) : perm(n, v, "NL") {}
	
	//Build permutation from tabular notation in a particular namespace
	//with a particular label
	perm(std::shared_ptr<nmap> n, std::vector<int> v, std::string label) : 
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
		
		int n_larger = std::max(mapping.size(), other.mapping.size());
		std::vector<int> newmapping(n_larger);
		//iota(begin(newtab), end(newtab), 0);
		
		for (int i = 0; i < n_larger; i++) {
			newmapping[i] = (i^*this)^other;
		}
		
		if (mapping.size() < other.mapping.size()) {	
			int i;	
			for (i = 0; i < int(mapping.size()); i++)
				newmapping[i] = other.mapping[mapping[i]];
			for (;i < int(other.mapping.size()); i++)
				newmapping[i] = other.mapping[i];
		} else {	
			int i;
			for (i = 0; i < n_larger; i++) {
				int tmp = mapping[i];
				if (tmp >= int(other.mapping.size())) newmapping[i] = tmp;
				else newmapping[i] = other.mapping[tmp];
			}
		}
		
		//cout << "\tnewmapping = " << newmapping << el;
		std::string newname = label;
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
		std::vector<int> newmapping(mapping.size());
		for (int i = 0; i < int(mapping.size()); i++)
			newmapping[mapping[i]] = i;
		mapping = newmapping;
		return *this;
	}
};

std::vector<perm> orbit(int pos, std::vector<perm> gens);

std::vector<perm> orbit(std::string label, std::vector<perm> gens);

std::ostream& operator<< (std::ostream& o, perm const& p);

template <>
std::ostream& operator<< <perm> (std::ostream& o, std::vector<perm> const& v);

#endif
