#include <string>
#include <iostream>
#include <vector>
#include <optional>
#include <cctype>
#include <functional>
#include "sparse.h"

// type abbreviations
using std::vector;
using std::string;
using std::optional;
using std::cin;
using std::cout;
using std::endl;

Parser one_char = [](const iter start, const iter end)
{
	bool done = false;

	return [done,start,end]() mutable {
		if (!done && start != end) {
			done = true;
			return optional<iter>(start+1);
		}
		return optional<iter>();
	};
};

Parser restrict(const Parser &parser, std::function<bool(const iter, const iter)> pred)
{
	return [parser,pred](const iter start, const iter end) {
		Result_iter i = parser(start, end);

		return [i,start,pred]() {
			for (optional<iter> res = i(); res; res = i())
				if (pred(start, *res))
					return res;
			return optional<iter>();
		};
	};
}

// Parses one letter.
Parser letter = restrict(one_char, [](const iter start, const iter end) { return std::isalpha(*start); });

// Parses one digit.
Parser digit = restrict(one_char, [](const iter start, const iter end) { return std::isdigit(*start); });

// The sum ("or") of two parsers.
Parser Either(const Parser &a, const Parser &b)
{
	return [a,b] (const iter start, const iter end) {
		Result_iter i = a(start, end);
		bool exhausted_a = false;
		bool exhausted_b = false;

		return [b,i,exhausted_a,exhausted_b,start,end] () mutable {
			while (!exhausted_b) {
				optional<iter> j = i();

				if (j)
					return j;

				if (!exhausted_a) {
					exhausted_a = true;
			      i = b(start, end);
				} else {
					exhausted_b = true;
				}
			}

			return optional<iter>();
		};
	};
}

// Convenient operator syntax for `Either` combinator.
Parser operator|(const Parser &a, const Parser &b)
{
	return Either(a, b);
}

// The zero parser that always fails.
Parser zero = [](const iter start, const iter end) {
	return []() {
		return optional<iter>();
	};
};

// The sum of a finite list of parsers.
Parser Any(const vector<Parser> parsers)
{
	Parser p = zero;

	for (vector<Parser>::const_reverse_iterator i = parsers.rbegin(); i != parsers.rend(); i++)
		p = Either(*i, p);

	return p;
}

// The product ("and") of two parsers.
Parser Both(const Parser &a, const Parser &b)
{
	return [a,b] (const iter start, const iter end) {
		Result_iter iter_a = a(start, end);
		optional<iter> start_b = iter_a();
		optional<Result_iter> iter_b;

		return [a,b,start,end,start_b,iter_a,iter_b] () mutable {
			while (start_b) {
				if (!iter_b)
					iter_b = b(*start_b, end);
				optional<iter> res = (*iter_b)();

				if (res) // Success
					return res;

				// Parser b couldn't succeed: let's try again with next result from a
				start_b = iter_a();
				iter_b = optional<Result_iter>();
			}

			// Parser a couldn't succeed: we're done
			return optional<iter>();
		};
	};
}

// Convenient operator syntax for `Both` combinator.
Parser operator&(const Parser &a, const Parser &b)
{
	return Both(a, b);
}

// The unit parser that only parses the empty string
// (always succeeds but never makes any progress).
Parser unit = [](const iter start, const iter end) {
	bool done = false;
	return [start,done]() mutable {
		if (done)
			return optional<iter>();
		done = true;
		return optional<iter>(start);
	};
};

// The product of a finite list of parsers.
Parser All(const vector<Parser> parsers)
{
	Parser p = unit;

	for (vector<Parser>::const_reverse_iterator i = parsers.rbegin(); i != parsers.rend(); i++)
		p = Both(*i, p);

	return p;
}

// Returns true iff the string `s` can be parsed successfully
// by parser `p` without residue.
bool valid(const Parser &p, const std::string &s)
{
	Result_iter i = p(s.begin(), s.end());
	optional<iter> res;
	
	for (res = i(); res && *res != s.end(); res = i());
	
	if (res)
		return true;
	else
		return false;
}