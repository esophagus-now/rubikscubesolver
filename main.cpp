#include <iostream>
#include <vector>
#include <string>
#include <set>
#include "nmap.h"
#include "perms.h"
#include "marco.h"

using namespace std;

perm cosrepinverse(int pos, int target, vector<perm> const& stree) {
	perm ret(stree[0]); //Create a perm in the same namespace
	
	while (pos != target) {
		perm const& tmp = stree[pos];
		ret *= tmp;
		pos ^= tmp;
	}
	
	return ret;
}

vector<perm> stabGens(int pos, vector<perm> const& gens) {
	auto stree = orbit(pos, gens);
	//cout << "stree.size() = " << stree.size() << el;
	
	set<perm> ret;
	
	for (auto const& r : stree) {
		for (auto const& s : gens) {
			perm tmp = r*s;
			perm rep = cosrepinverse(pos^tmp, pos, stree);
			tmp *= rep;
			
			auto it = ret.find(tmp);
			if (it != ret.end()) {
				//If the perm already in the set has a longer representation,
				//delete it
				if (it->numFactors() > tmp.numFactors()) {
					//cout << "\terasing " << *it << el;
					ret.erase(it);
				} 
			}
			//cout << "\tinserting " << tmp << el;
			ret.insert(tmp);
			//cout << "\t(new length = " << ret.size() << el;
		} 
	}
	//cout << "ret.size() = " << ret.size() << el;
	
	return vector<perm>(begin(ret), end(ret));
}

vector<perm> stabGens(string const& label, vector<perm> const& gens) {
	shared_ptr<nmap> names = gens[0].names;
	
	//ret will contain the generators
	vector<perm> ret(names->n, perm());
	
	if (!names->contains(label)) return ret; //gens is empty
	
	int pos = (*names)[label];
	
	return stabGens(pos, gens);
}

int main() {
	shared_ptr<nmap> rubiks = make_shared<nmap>();
	perm U(rubiks,"( 1, 3, 8, 6)( 2, 5, 7, 4)( 9,33,25,17)(10,34,26,18)(11,35,27,19)", "U");
	perm L(rubiks,"( 9,11,16,14)(10,13,15,12)( 1,17,41,40)( 4,20,44,37)( 6,22,46,35)", "L");
	perm F(rubiks,"(17,19,24,22)(18,21,23,20)( 6,25,43,16)( 7,28,42,13)( 8,30,41,11)", "F");
	perm R(rubiks,"(25,27,32,30)(26,29,31,28)( 3,38,43,19)( 5,36,45,21)( 8,33,48,24)", "R");
	perm B(rubiks,"(33,35,40,38)(34,37,39,36)( 3, 9,46,32)( 2,12,47,29)( 1,14,48,27)", "B");
	perm D(rubiks,"(41,43,48,46)(42,45,47,44)(14,22,30,38)(15,23,31,39)(16,24,32,40)", "D");
	
	vector<perm> gens = {perm(rubiks),U,L,F,R,B,D};
	
	vector<string> toStab = {"1","2","3","4","5","6","7","8","12","20","28","36","41","42","43","44","45","46"};
	vector<vector<perm> > grpgens(toStab.size());
	
	for (auto const& s : toStab) {
		cout << "Stabilizing up to element " << s << ":" << el;
		gens = stabGens(s, gens);
		cout << gens.size() << el;
		grpgens.push_back(gens);
	}
	
}
