#include <string>
#include <iostream>
#include "sparse.h"

using std::string;
using std::cin;
using std::cout;
using std::endl;

Parser ab = [](const iter start, const iter end) {
	Parser a = 'a'_P;
	Parser b = 'b'_P;

	return (unit | (a & ab & b & ab) | (b & ab & a & ab))(start, end);
};

// Trying to use this parser results in std::bad_function_call.
// TODO: Fix this.
Parser ba = unit | ('a'_P & ba & 'b'_P & ba) | ('b'_P & ba & 'a'_P & ba);

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
