#include <string>
#include <iostream>
#include "sparse.h"

using std::string;
using std::cin;
using std::cout;
using std::endl;

Parser ab = [](const iter start, const iter end) {
	Parser a = restrict(one_char, [](const iter start, const iter end) { return *start == 'a';});
	Parser b = restrict(one_char, [](const iter start, const iter end) { return *start == 'b';});

	return (unit | (a & ab & b & ab) | (b & ab & a & ab))(start, end);
};

int main()
{
	string l;

	while (cin >> l) {
		if (valid(ab, l))
			cout << "ACCEPTED" << endl;
	   else
			cout << "REJECTED" << endl;
	}

	return 0;
}