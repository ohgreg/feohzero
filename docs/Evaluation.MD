## Basic Idea 

In order to finally search through all of the possible legal moves, we need to be able to judge how beneficial a move is for the player. In order to do so, we're going to need to evaluate a chess position to figure out which player is winning.
Evaluating a chess position can be really complex. In real analysis of a position, one must take into account a plethora of factors, such as total material, development of pieces, king safety, and much more. 

FeohZero analyses a position based on:
- Material count
- Quality of each piece 
- Game Phase (Tapered eval) 
- Doubled pawns 
- King safety 

## Material Count and PSTs

Material count is pretty straight forward. We assign a certain value to each piece and sum these values for each side. The quality of each piece is judged through the use of [Piece-Square Tables](https://www.chessprogramming.org/Piece-Square_Tables). 
These are 64 integer arrays that store a value for each piece, for each square. This means that we assign a bonus or a penalty to each piece, depending on its position on the board. For example, a knight on the edge of the board is deemed to be quite bad,
so we assign a penalty to it. If it's closer to the center, we deem it to be better, so we assign it a bonus. The same logic is done for each piece. 

## Tapered Eval
We actually have 2 of these tables for each piece, an MG (middle game) table and an EG (endgame) table, in order to more accurately assign these bonuses based on the phase of the game. This is more well-known as 
[Tapered Eval](https://www.chessprogramming.org/Tapered_Eval#:~:text=a%20technique%20used%20in%20evaluation,of%20captured%20pieces%20so%20far). The Game Phase is calculated by giving a game_phase_value to each piece and adding them.
For example, a queen is worth 4 game_phase_points, and so it brings the "phase" of the game closer to a middlegame than an endgame. In the end, we will use a simple formula to give more "significance" to either the middle game or endgame table.


## Sources
