# Description 

FeohZero is a simple [Chess Engine](https://en.wikipedia.org/wiki/Chess_engine) written in C. Its usage is quite simple. The main program (engine.c) takes 3 arguments. These are: The [FEN](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation) 
that describes the chess position, a list of the moves that the engine is allowed to play, as well as an integer value that indicates the maximum thinking time of the engine. If the program is used with correct arguments, it will print a single integer: The index that
corresponds to the move the engine chose out of the list (Indexing starts at 0) and it will terminate with exit code `0`. If the correct arguments arent given, instructions for its usage is given, and it returns with exit code `1`. Should it fail for any other reason,
it will again exit with exit code `1`.

# Features

Some basic features of the engine are: 
- Bitboards & Magic Bitboards
- Material & Piece-Square Table Evaluation
- King Safety & Pawn structure evaluation
- Iterative Deepening Search
- AB-Pruning
- Move Ordering & PV-Node
- Transposition Table

Note: Details on the implementation of the engine are included in the Docs folder in an organized manner.

# Demo

```sh
$ make engine
gcc -Wall -Wextra -Werror -pedantic -Iinclude -O3 -Ilibs/unity -c src/engine.c -o build/engine.o
gcc -Wall -Wextra -Werror -pedantic -Iinclude -O3 -Ilibs/unity -c src/moves.c -o build/moves.o
gcc -Wall -Wextra -Werror -pedantic -Iinclude -O3 -Ilibs/unity -c src/eval.c -o build/eval.o
gcc -Wall -Wextra -Werror -pedantic -Iinclude -O3 -Ilibs/unity -c src/search.c -o build/search.o
gcc -Wall -Wextra -Werror -pedantic -Iinclude -O3 -Ilibs/unity -c src/zobrist.c -o build/zobrist.o
gcc -Wall -Wextra -Werror -pedantic -Iinclude -O3 -Ilibs/unity -c src/transposition.c -o build/transposition.o
gcc -Wall -Wextra -Werror -pedantic -Iinclude -O3 -Ilibs/unity build/engine.o build/fen.o build/moves.o build/print.o build/board.o build/eval.o build/search.o build/zobrist.o build/transposition.o -o engine
$ ./engine "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" "a3 a4 b3 b4 c3 c4 d3 d4 e3 e4 f3 f4 g3 g4 h3 h4 Na3 Nc3 Nf3 Nh3" 3     
16
```

[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/KoToqcBN)

# Project installation

## Docker setup
Build and run the development environment container, by running the command:
```sh
docker compose up -d --build
```
You should now be able to interact with your engine on http://0.0.0.0:8000/.

Now, if you want to recompile the sources, just firstly attach to the project's
running container:
```sh
docker exec -it feoh bash
```
and then run the commands:
```sh
make engine
```
if you want to build the c program, or:
```sh
make web/engine.wasm
```
if you want to build the web assembly program (also consider refreshing the browser).

Lastly, run:
```sh
docker compose down
```
to just shut down the container.

## Manual setup (Linux or MacOS)

Firstly, if you want to build the C program, run:
```sh
make engine
```

If you want to run the server, compile the webassembly target (requires the [emscripten compiler](https://emscripten.org/docs/getting_started/downloads.html#sdk-download-and-install])), and then run:

```sh
make WEB_TARGET=web/engine.wasm web/engine.wasm
```

Finally, run the server (requires python3):

```sh
$ make run
python3 -m http.server --directory web
Serving HTTP on 0.0.0.0 port 8000 (http://0.0.0.0:8000/) ...
```

You should now be able to interact with your engine on http://0.0.0.0:8000/.

![Engine Screenshot](web/img/screenshot.png)
