# `sparse` - simple parser

`sparse` is a simple (toy) parser combinatory library for educational purposes.

# Requirements

- `Makefile`
- `gcc` supporting the flag `-std=c++17`

# Building and Usage

Running

    make
	 
in the source directory will create the files `sparse.a` and
`ab`.

The first is a static version of the combinator library and the second is
a simple interactive recognizer for the familiar "ab language" (strings
comprised of the letters a and b with equal frequency).

Run it with

    ./ab

and have fun.

