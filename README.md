# FeohZero

A chess engine written in C, developed as a [pair](https://github.com/ItsFeold) homework assignment for the [Introduction to Programming](https://progintro.github.io) course at [DIT NKUA](https://www.di.uoa.gr/en). 1st place winner of the [hw3-2024](https://progintro.github.io/past/2024/assets/pdf/hw3.pdf) class competition.

This repository provides an enhanced version of the original assignment submission; it fixes known bugs and makes it easier to test and experiment with the engine.

## Features

The main features of the engine are:

- Board representation: utilizes [Bitboards](https://www.chessprogramming.org/Bitboards) and [Magic Bitboards](https://www.google.com/search?client=safari&rls=en&q=magic+bitboards&ie=UTF-8&oe=UTF-8) for high-performance move generation; correctness was fully verified via [Perft tests](https://www.chessprogramming.org/Perft_Results).

- Search: employs [Alpha-beta pruning](https://en.wikipedia.org/wiki/Alpha–beta_pruning) with [Iterative Deepening Search](https://en.wikipedia.org/wiki/Iterative_deepening_depth-first_search) (IDS) and [Principal Variation](https://www.chessprogramming.org/Principal_Variation) (PV) move ordering. [Zobrist Hashing](https://en.wikipedia.org/wiki/Zobrist_hashing) is used for position identification and efficient [Transposition Table](https://en.wikipedia.org/wiki/Transposition_table#:~:text=A%20transposition%20table%20is%20a,game%20tree%20below%20that%20position.) lookups.

- Evaluation: a hand-crafted evaluation function combining material weights, [Piece-Square Tables](https://www.chessprogramming.org/Piece-Square_Tables) (PST), king safety, and pawn structure.

## Build

Run `make` to build the engine binary (named `feohzero`).
```sh
make
```

To clean the build, run `make clean`.

**Thanks to [0xJoeMama](https://github.com/0xJoeMama) for improving the original Makefile; highly recommend checking out [*How to Make?*](https://progintro.github.io/assets/pdf/make.pdf).**

## Usage

FeohZero searches for the best move in a given chess position in [FEN format](https://en.wikipedia.org/wiki/Forsyth–Edwards_Notation).

```bash
./feohzero [fen] [options]
```

Below are some examples on how to run the engine (positions are from [Game 1](https://www.youtube.com/watch?v=SMe-hvCwTRo) and [Game 2](https://www.youtube.com/watch?v=MRoTJntvv_8)):

```bash
# show help message
./feohzero --help

# search from starting position
./feohzero

# search from a specific position
./feohzero "r3qb1k/1b4p1/p2pr2p/3n4/Pnp1N1N1/6RP/1B3PP1/1B1QR1K1 w - - 4 26"

# set search depth and time limit
./feohzero --depth 9 --timeout 5000

# search only specific moves
./feohzero --moves "Nc3 Nf3 d4 e4"

# use UCI notation
./feohzero --notation uci --moves "b1c3 g1f3 d2d4 e2e4"

# quiet mode (only output best move)
./feohzero --quiet --depth 10 --timeout 10000

# full example
./feohzero "r3r1k1/pp3pbp/1Bp1b1p1/8/2BP4/Q1n2N2/P4PPP/3R1K1R b - - 0 18" --depth 8 --timeout 3000 --notation uci
```

The engine can be configured using the following cli flags:

| Option | Description | Default |
|--------|-------------|---------|
| `--moves` | moves to search, separated by spaces | all possible moves |
| `--depth` | maximum search depth | 10 |
| `--timeout` | search time limit in ms | 1000 |
| `--notation` | move notation format, `san` or `uci` | `san` |
| `--tt-size` | transposition table size in MB | 16 |
| `--seed` | seed for RNG | 259 |
| `--quiet` | only output the final best move | - |
| `--help` | show help message | - |



## Known limitations & future work

The current limitations of the engine are:

- UCI protocol: not currently compliant; the engine does not yet support standard GUI communication.

- Knowledge gaps: lacks an opening book and specialized endgame logic or tablebase support.

- Single-threaded: search is limited to a single core with no support for parallel processing.

Some future improvements planned for the project include:

- Protocol support: implementing the UCI protocol to allow compatibility with standard chess GUIs.

- Parallelism: transitioning to multi-threaded search using Lazy SMP.

- Databases: adding support for Polyglot opening books and Syzygy endgame tablebases.

- Web integration: compiling to WebAssembly (WASM) to host the engine as an interactive online web app.
