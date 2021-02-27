#include <string>
#include <iostream>
#include "sparse.h"

using std::string;
using std::cin;
using std::cout;
using std::endl;

Parser ab = ETA(unit | ('a'_P & ab & 'b'_P & ab) | ('b'_P & ab & 'a'_P & ab));

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
