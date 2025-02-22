## Basic Idea

First, it should be established that a chess game can be represented as a massive tree of a very large number of nodes, where each node represents a chess position, and all the legal moves at that position lead to the children nodes, which are new chess positions.
Searching for the "sub-tree" of moves that lead to the most beneficial outcome for the current player is the core of every chess engine, and at the heart of this search algorithm is [Minimax](https://en.wikipedia.org/wiki/Minimax). 

## Minimax

Minimax is a recursive algorithm which calculates the best possible outcome for one player by looking through all the possible sub-trees of moves and returning the one that leads to the most beneficial static evaluation of a chess position, at a given depth. 
The 
