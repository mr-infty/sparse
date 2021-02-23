CC = g++
FLAGS = "-std=c++1z"
OBJ = sparse.a ab

.PHONY: all clean

all: $(OBJ)

clean:
	rm -f $(OBJ)

ab: sparse.a

%: %.cpp
	$(CC) $(FLAGS) $^ -o $@

%.o: %.cpp
	$(CC) $(FLAGS) -c $< -o $@

%.a: %.o
	$(AR) rsv $@ $^
