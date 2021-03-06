#include <string>
#include <vector>
#include <optional>
#include <cctype>
#include <functional>
#include "sparse.h"

// type abbreviations
using std::vector;
using std::string;
using std::optional;

Parser one_char = [](iter start, iter end)
{
	bool done = false;

	return [=]() mutable {
		if (!done && start != end) {
			done = true;
			return optional<iter>(start+1);
		}
		return optional<iter>();
	};
};

Parser restrict(const Parser &parser, std::function<bool(iter, iter)> pred)
{
	return [=](iter start, iter end) {
		Result_iter i = parser(start, end);

		return [=]() {
			for (optional<iter> res = i(); res; res = i())
				if (pred(start, *res))
					return res;
			return optional<iter>();
		};
	};
}

Parser letter = restrict(one_char, [](iter start, iter end) { return std::isalpha(*start); });

Parser digit = restrict(one_char, [](iter start, iter end) { return std::isdigit(*start); });

Parser Either(const Parser &a, const Parser &b)
{
	return [=] (iter start, iter end) {
		Result_iter i = a(start, end);
		bool exhausted_a = false;
		bool exhausted_b = false;

		return [=] () mutable {
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

Parser operator|(const Parser &a, const Parser &b)
{
	return Either(a, b);
}

Parser zero = [](iter start, iter end) {
	return []() {
		return optional<iter>();
	};
};

Parser Any(const vector<Parser> &parsers)
{
	Parser p = zero;

	for (vector<Parser>::const_reverse_iterator i = parsers.rbegin(); i != parsers.rend(); i++)
		p = Either(*i, p);

	return p;
}

Parser Both(const Parser &a, const Parser &b)
{
	return [=] (iter start, iter end) {
		Result_iter iter_a = a(start, end);
		optional<iter> start_b = iter_a();
		optional<Result_iter> iter_b;

		return [=] () mutable {
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

Parser operator&(const Parser &a, const Parser &b)
{
	return Both(a, b);
}

Parser unit = [](iter start, iter end) {
	bool done = false;
	return [=]() mutable {
		if (done)
			return optional<iter>();
		done = true;
		return optional<iter>(start);
	};
};

Parser All(const vector<Parser> &parsers)
{
	Parser p = unit;

	for (vector<Parser>::const_reverse_iterator i = parsers.rbegin(); i != parsers.rend(); i++)
		p = Both(*i, p);

	return p;
}

Parser OneOrMore(const Parser &a)
{
	return a | OneOrMore(a);
}

Parser operator +(const Parser &a)
{
	return OneOrMore(a);
}

Parser ZeroOrMore(const Parser &a)
{
	return unit | OneOrMore(a);
}

Parser ZeroOrOne(const Parser &a)
{
	return unit | a;
}

Parser Power(const Parser &a, unsigned int n)
{
	if (n == 0)
		return unit;
	else if (n == 1)
		return a;
	else
		return a & Power(a, n-1);
}

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

Parser operator ""_P(char c)
{
	return restrict(one_char, [c](iter start, iter end) { return *start == c;});
}

Parser operator ""_P(const char * s, size_t len)
{
	return [=](iter start, iter end) {
		bool done = false;
		bool found = string(start,end).rfind(s, 0, len) == 0;

		return [=]() mutable {
			if (!done && found) {
				done = true;
				return optional<iter>(start+len);
			}
			return optional<iter>();
		};
	};
}
