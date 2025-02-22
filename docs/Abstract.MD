## Abstract ##

The "Documents" folder contains MD (Markdown) files that describe the process followed for the implementation of our chess engine, FeohZero (written in C). It also references sources that were used during the implementation, as well possible improvements that could be made. 

In summary, the implementation of the engine consists of the following parts: 
- <u>Modeling:</u> A foundamental task where we have to represent the chess board in code, as well as the various chess rules
- <u>Move Generation:</u> All possible (legal) moves have to be generated in order eventually choose one of them
- <u>Evaluation:</u> In order to judge the strength of a move, the static evaluation of a chess position must be calculated, or in other words, which player the current position favours the most
- <u>Search:</u> Searching through all the generated moves in order to find the one that favours the player the most, using the aforementioned evaluation.

