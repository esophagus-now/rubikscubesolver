#ifndef MARCO_H
#define MARCO_H 1

#include <iostream>
#include <vector>
#include <map>

std::ostream& el(std::ostream& o);

template <typename T>
std::ostream& operator<< (std::ostream& o, std::vector<T> const& v) {
	o << "[";
	auto delim = "";
	for (auto const& i: v) {
		o << delim << i;
		delim = ",";
	}
	return o << "]";
}

template <typename T, typename U>
std::ostream& operator<< (std::ostream& o, std::map<T,U> const& m) {
	o << "[";
	auto delim = "";
	for (auto const& i: m) {
		o << delim << i.first << "=" << i.second;
		delim = "|";
	}
	return o << "]";
}

#endif
