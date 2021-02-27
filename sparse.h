#pragma once

#include <string>
#include <functional>
#include <optional>

// GENERAL REMARKS
//
// This library is a parser *combinator* library,
// which means that:
//  a) parsers are re-ified as objects, and
//  b) that as such they are constructed by applying
//     a finite set of *combinators* repeatedly to some
//     elementary parsers.
//
// PARSERS AS OBJECTS
//
// Parsers are interpreted here as function objects
// of type (pseudo-type theoretical notation)
//
//     parser: iter -> iter -> Result_iter.
// 
// Here, the first two arguments of type `iter` denote
// the start and end of the string to be parsed, and 
// the return type is the type of iterators over parsing results.
//
// More precisely, a value of type `Result_iter` is a function
// object of type
//
//     i : std::optional<iter>.
//
// Thus, `i()` is a value of type `std::optional<iter>`. Such a result
// iterator `i` is expected to conform to the following requirements:
//
//   I) If the result of a function call contains no value (i.e. evaluates
//      to false as a boolean), then the results of all further function
//      calls don't contain a value either.
//   II) If `i()` contains a value, then the value `j = *i()` of type `iter`
//       shall be such that the range [start, j[ denotes a valid string for
//       that parser.
//   III) All initial substrings that are valid for this parser shall be
//        obtained by the procedure outlined in II), and there shall be
//        no duplicates. More precisely, the value `j = *i()` is expected
//        to increase strictly monotonously with each call.

// TYPE DEFINITIONS.
typedef std::string::const_iterator iter;
typedef std::function<std::optional<iter>()> Result_iter;
typedef std::function<Result_iter(iter, iter)> Parser;

// ELEMENTARY PARSERS.

// Parses one char.
extern Parser one_char;

// Parses one letter.
extern Parser letter;

// Parses one digit.
extern Parser digit;

// The zero parser that always fails.
extern Parser zero;

// The unit parser that only parses the empty string
// (always succeeds but never makes any progress).
extern Parser unit;

// PARSER COMBINATORS.

// Restricts a parser to only return results satisfying a predicate.
Parser restrict(const Parser &parser, std::function<bool(iter, iter)> pred);

// The sum ("or") of two parsers.
Parser Either(const Parser &a, const Parser &b);

// Convenient operator syntax for `Either` combinator.
Parser operator|(const Parser &a, const Parser &b);

// The sum of a finite list of parsers.
Parser Any(const std::vector<Parser> &parsers);

// The product ("and") of two parsers.
Parser Both(const Parser &a, const Parser &b);

// Convenient operator syntax for `Both` combinator.
Parser operator&(const Parser &a, const Parser &b);

// The product of a finite list of parsers.
Parser All(const std::vector<Parser> &parsers);

// CONVENIENCE FUNCTIONS.

// eta expansion of Parser-valued expressions for conventient definition of
// recursive parsers.
//
// E.g.: Parser ab = ETA(unit | 'a'_P & ab & 'b'_P & ab | 'b'_P & ab & 'a'_P
// & ab);
#define ETA(x) ([](iter start, iter end) { return (x)(start, end); })

// Returns true iff the string `s` can be parsed successfully
// by parser `p` without residue.
bool valid(const Parser &p, const std::string &s);

// User-defined literal operator for parsing a given char.
Parser operator ""_P(char);

// User-defined literal operator for parsing a string literal.
Parser operator ""_P(const char *, std::size_t);
