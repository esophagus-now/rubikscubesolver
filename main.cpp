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
		if (!tmp) {
			return ret;
		}
		ret *= tmp;
		pos ^= tmp;
	}
	
	return ret;
}

vector<perm> shreierGens(int pos, vector<perm> const& stree, vector<perm> const& gens) {
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
			ret.insert(tmp);
		} 
	}
	
	return vector<perm>(begin(ret), end(ret));
}

vector<perm> shreierGens(string const& label, vector<perm> const& gens) {
	shared_ptr<nmap> names = gens[0].names;
	
	//ret will contain the generators
	vector<perm> ret(names->n, perm());
	
	if (!names->contains(label)) return ret; //gens is empty
	
	int pos = (*names)[label];
	
	auto stree = orbit(pos, gens);
	
	return shreierGens(pos, stree, gens);
}

struct stabchain {
	vector<int> pnts;
	vector<vector<perm> > stabgens;
	vector<vector<perm> > strees;
	
	//Makes sure that when a stabchain is created, the caller intializes
	//it in a valid way
	stabchain() = delete;
	
	stabchain(vector<perm> const& gens) {
		pnts.push_back(-1); //Need some kind of dummy
		stabgens.push_back(gens);
		strees.push_back({}); //Push empty vector
	}
};

perm sift(perm const& p, stabchain const& s, int level) {
	int target = s.pnts[level]; //Base point
	perm ratio = p;
	while (!ratio.isIdentity()) {
		int pos = target^p;
		perm rep = cosrepinverse(pos, target, s.strees[level]);
		if (rep.isIdentity()) {
			return ratio;
		}
		ratio *= rep;
	}
	
	return ratio;
}

//Starting at level, build the stabilizer chain "all the way down" to
//the trivial group. May decide on new base points.
void shreiersims(stabchain &s, int level) {
	//cout << "Called at level " << level << el;
	if (level <= 0) {
		cout << "Possible error: shreiersims should not deal with this level" << el;
		return;
	}
	
	//Make sure the stabchain object has space allocated for our stuff
	
	// 1. Initialize generator set (if necessary)
	auto gens = s.stabgens[level - 1];
	if ((int)s.stabgens.size() == level) {
		//Initialize generator set at next level to just the identity
		s.stabgens.push_back({perm(gens[0])}); //Needed an identity perm here
		//return;
	} else if ((int)s.stabgens.size() < level) {
		cout << "Error: attempted to use a level value too far from the rest" << el;
		return;
	}
	
	// 2. Make space for the stree
	if ((int)s.strees.size() == level) {
		s.strees.resize(level+1); //Expand vector to make space
	} else if ((int)s.strees.size() < level) {
		cout << "Error: attempted to use a level value too far from the rest (stree)" << el;
		return;
	}
	
	// 3. Choose a base point (if necessary)
	if ((int)s.pnts.size() == level) {
		int cur;
		//Search for first non-identity permutation in gens
		for (auto const& p : gens) {
			if ((cur = p.firstMover()) != -1) {
				break;
			}
		}
		if (cur == -1) {
			cout << "Error: somehow we're trying to stabilize points"
					" in the trivial group" << el;
			return;
		}
		s.pnts.push_back(cur); //Adds cur to base
		cout << "*************************************" << el;
		cout << "\tpnts is now " << s.pnts << el;
		cout << "*************************************" << el;
	} else if ((int)s.pnts.size() < level) {
		cout << "Error: attempted to use a level value too far from the rest (pnts)" << el;
		return;
	}
	
	//Now move on to the algorithm proper
	
	int curbase = s.pnts[level];
	
	//Also keep track of the shreier tree (this is for sifting)
	auto stree = orbit(curbase, gens);
	s.strees[level] = stree; //stree already a vector<perm>
	
	auto sgens = shreierGens(curbase, stree, gens);
	//TODO: Can be smart about not redoing work for generators we already
	//had before. That is, if the existing shreier generators were {a,b,c}
	//and the new ones are {a,b,c,d} then we only need to check if d is
	//redundant (since we have already checked a, b, and c).
	
	//Check for redundant generators
	for (auto const& g : sgens) {
		perm siftee = sift(g, s, 1);
		if (siftee.isIdentity()) {
			//This generator is redundant
			continue;
			//If we hit this case for each generator, then that would be
			//the base case
		} else {
			//Add this generator to the set
			s.stabgens[level].push_back(g);
			if (level <= 4) cout << "\tAdding " << g << " to level " << level << el;
			//"Trickle down" the new generator
			shreiersims(s, level + 1);
		}
	}
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
	
	stabchain s(gens);
	
	shreiersims(s, 1);
	
	cout << s.pnts << el;
}
