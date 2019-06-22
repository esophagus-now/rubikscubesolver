#include <iostream>
#include <vector>
#include <string>
#include "nmap.h"
#include "perms.h"
#include "marco.h"

using namespace std;

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
}
