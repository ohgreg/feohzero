import { useState, useEffect, useRef, useCallback, useMemo } from "react";
import { Chess, validateFen } from "chess.js";
import {
  Chessboard,
  chessColumnToColumnIndex,
  defaultPieces,
} from "react-chessboard";
import type { PieceHandlerArgs, PieceDropHandlerArgs } from "react-chessboard";
import chessEngine from "./wasm/engine";

import AboutPopUp from "./components/AboutPopUp";
import SettingsPopUp from "./components/SettingsPopUp";
import FENPopup from "./components/FENPopUp";
import TurnIndicator from "./components/TurnIndicator";
import {
  Play,
  Copy,
  First,
  Last,
  Pause,
  Previous,
  Reset,
  Settings,
  Flip,
  Next,
  Random,
  Set,
  Export,
  Github,
  Info,
} from "./components/Icons";

import "./styles/main.scss";

function App() {
  /* game state */
  const gameRef = useRef(new Chess());
  const game = gameRef.current;
  const [position, setPosition] = useState<string>(game.fen());
  const [initialFen, setInitialFen] = useState<string>(game.fen());
  const [boardOrientation, setBoardOrientation] = useState<"white" | "black">(
    "white",
  );
  const [winner, setWinner] = useState<"white" | "black" | "draw" | null>(null);
  const [redoStack, setRedoStack] = useState<
    Array<{ from: string; to: string; promotion?: string }>
  >([]);

  /* game controls */
  const [isStarted, setIsStarted] = useState<boolean>(false);
  const [playRandom, setPlayRandom] = useState<boolean>(false);
  const [currentTimeout, setCurrentTimeout] = useState<NodeJS.Timeout>();

  /* handle promotions */
  const [promotionMove, setPromotionMove] = useState<Omit<
    PieceDropHandlerArgs,
    "piece"
  > | null>(null);

  /* engine state */
  const [engineInitialized, setEngineInitialized] = useState<boolean>(false);
  const [isEngineThinking, setIsEngineThinking] = useState<boolean>(false);
  const [searchInfo, setSearchInfo] = useState<{
    depth?: number;
    nodes?: number;
    time?: number;
    nps?: number;
  }>({});
  const [engineSettings, setEngineSettings] = useState({
    depth: 8,
    timeLimit: 3000,
    ttSize: 16000,
  });

  /* pop-ups state */
  const [isAboutPopUpOpen, setIsAboutPopUpOpen] = useState(false);
  const [isSettingsPopupOpen, setIsSettingsPopupOpen] =
    useState<boolean>(false);
  const [isFENPopupOpen, setIsFENPopupOpen] = useState<boolean>(false);

  const setPositionWrapper = useCallback(
    (newPosition: string) => {
      setPosition(newPosition);
      if (game) {
        if (game.isCheckmate()) {
          setIsStarted(false);
          setWinner(game.turn() === "w" ? "black" : "white");
        } else if (game.isDraw() || game.isThreefoldRepetition()) {
          setIsStarted(false);
          setWinner("draw");
        } else {
          setWinner(null);
        }
      }
    },
    [game],
  );

  const makeRandomMove = useCallback(() => {
    if (game.isGameOver()) {
      return;
    }

    const possibleMoves = game.moves();
    const move =
      possibleMoves[Math.floor(Math.random() * possibleMoves.length)];
    if (move) {
      setRedoStack([]);
      game.move(move);
    }

    setPositionWrapper(game.fen());
  }, [game, setPositionWrapper]);

  const makeEngineMove = useCallback(async () => {
    if (!engineInitialized || game.isGameOver()) {
      return;
    }

    setIsEngineThinking(true);

    await new Promise((resolve) => requestAnimationFrame(resolve));

    try {
      chessEngine.loadFEN(game.fen());

      const result = await new Promise<{
        move: string;
        depth: number;
        time: number;
        nodes: number;
        nps: number;
        timeout: boolean;
      }>((resolve) => {
        setTimeout(() => {
          const searchResult = chessEngine.search(
            engineSettings.depth,
            engineSettings.timeLimit,
          );
          resolve(searchResult);
        }, 0);
      });

      setSearchInfo({
        depth: result.depth,
        nodes: result.nodes,
        time: result.time,
        nps: result.nps,
      });

      if (result.move && result.move !== "none") {
        const from = result.move.slice(0, 2);
        const to = result.move.slice(2, 4);
        const promotion = result.move.length === 5 ? result.move[4] : undefined;

        const move = game.move({ from, to, promotion });

        if (move) {
          setRedoStack([]);
          setPositionWrapper(game.fen());
        }
      }
    } catch (error) {
      console.error("Engine move failed:", error);
    } finally {
      setIsEngineThinking(false);
    }
  }, [engineInitialized, game, setPositionWrapper, engineSettings]);

  useEffect(() => {
    const initEngine = async () => {
      try {
        await chessEngine.initialize(engineSettings.ttSize);
        setEngineInitialized(true);
      } catch (error) {
        console.error("Failed to initialize engine:", error);
      }
    };

    initEngine();
  }, [engineSettings.ttSize]);

  useEffect(() => {
    return () => {
      if (currentTimeout) {
        clearTimeout(currentTimeout);
      }
    };
  }, [currentTimeout]);

  useEffect(() => {
    if (!isStarted || game.isGameOver() || isEngineThinking) return;

    const turn = game.turn();
    const playerColor = boardOrientation === "white" ? "w" : "b";

    if (turn !== playerColor) {
      if (!engineInitialized) {
        const timeout = setTimeout(makeRandomMove, 500);
        setCurrentTimeout(timeout);
        return () => clearTimeout(timeout);
      } else {
        const timeout = setTimeout(() => {
          makeEngineMove();
        }, 200);
        setCurrentTimeout(timeout);
        return () => clearTimeout(timeout);
      }
    } else if (playRandom) {
      const timeout = setTimeout(makeRandomMove, 500);
      setCurrentTimeout(timeout);
      return () => clearTimeout(timeout);
    }
  }, [
    isStarted,
    position,
    isEngineThinking,
    engineInitialized,
    playRandom,
    boardOrientation,
    makeEngineMove,
    makeRandomMove,
    game,
  ]);

  const onPieceDrop = ({
    sourceSquare,
    targetSquare,
  }: PieceDropHandlerArgs) => {
    if (!isStarted || !game || !targetSquare) return false;

    if (!playRandom && game.turn() !== boardOrientation[0]) return false;

    const piece = game.get(sourceSquare);
    const rank = targetSquare[1];

    if (
      piece &&
      piece.type === "p" &&
      ((piece.color === "w" && rank === "8") ||
        (piece.color === "b" && rank === "1"))
    ) {
      const moves = game.moves({ square: sourceSquare, verbose: true });
      if (moves.some((m) => m.to === targetSquare && m.promotion)) {
        setPromotionMove({ sourceSquare, targetSquare });
        return true;
      }
    }

    try {
      const move = game.move({
        from: sourceSquare,
        to: targetSquare,
        promotion: "q",
      });

      if (move) {
        setRedoStack([]);
        setPositionWrapper(game.fen());
        return true;
      }
      return false;
    } catch (error) {
      return false;
    }
  };

  const boardPause = () => {
    setIsStarted(false);

    setSearchInfo({});

    if (currentTimeout) {
      clearTimeout(currentTimeout);
      setCurrentTimeout(undefined);
    }
  };

  const boardFlip = () => {
    setBoardOrientation((prev) => (prev === "white" ? "black" : "white"));
  };

  const boardReset = () => {
    boardPause();
    setWinner(null);
    setRedoStack([]);

    if (game) {
      game.reset();
      setInitialFen(game.fen());
      setPositionWrapper(game.fen());
    }

    if (engineInitialized) {
      chessEngine.clearTT();
    }
  };

  const boardTogglePlayPause = () => {
    setIsStarted(!isStarted);
  };

  const boardUndo = () => {
    boardPause();

    if (game.history().length > 0) {
      const move = game.undo();

      if (move) {
        setRedoStack((prev) => [...prev, move]);
        setPositionWrapper(game.fen());
      }
    }
  };

  const boardRedo = () => {
    boardPause();

    if (redoStack.length > 0) {
      const redoMove = redoStack[redoStack.length - 1];
      setRedoStack((prev) => prev.slice(0, -1));
      game.move(redoMove);
      setPositionWrapper(game.fen());
    }
  };

  const boardFirst = () => {
    boardPause();

    setRedoStack([...game.history({ verbose: true })].reverse());
    game.load(initialFen);
    setPositionWrapper(game.fen());
  };

  const boardLast = () => {
    boardPause();

    const movesToPlay = [...redoStack];
    setRedoStack([]);

    movesToPlay.reverse().forEach((move) => {
      game.move(move);
    });

    setPositionWrapper(game.fen());
  };

  const boardTogglePlayRandom = () => {
    setPlayRandom(!playRandom);
  };

  const boardExportFEN = () => {
    navigator.clipboard.writeText(game.fen());
  };

  const boardHandleFENSubmit = (fen: string) => {
    boardPause();
    if (game) {
      if (validateFen(fen).ok == true) {
        setRedoStack([]);
        game.load(fen);
        setInitialFen(fen);
        setPositionWrapper(game.fen());
        setIsFENPopupOpen(false);
      } else {
        alert("Invalid FEN string. Please try again.");
      }
    }
  };

  const boardHandleSettingsSubmit = async (newSettings: {
    depth: number;
    timeLimit: number;
    ttSize: number;
  }) => {
    const ttChanged = newSettings.ttSize !== engineSettings.ttSize;

    setEngineSettings(newSettings);

    if (ttChanged) {
      setEngineInitialized(false);
      try {
        await chessEngine.initialize(newSettings.ttSize);
        setEngineInitialized(true);
        console.log("Engine reinitialized with new TT size");
      } catch (error) {
        console.error("Failed to reinitialize engine:", error);
      }
    }
  };

  const boardExportPGN = () => {
    game.removeHeader("Event");
    game.setHeader("Round", "1");
    game.setHeader("Event", "Online match with FeohZero engine");
    game.setHeader("Site", "ohgreg.github.io/feohzero");
    game.setHeader(
      "Date",
      new Date().toLocaleDateString("sv-SE").replace(/-/g, "."),
    );
    game.setHeader(
      "White",
      boardOrientation === "white" ? "Player" : "FeohZero",
    );
    game.setHeader(
      "Black",
      boardOrientation === "black" ? "Player" : "FeohZero",
    );
    game.setHeader(
      "Result",
      winner === "white"
        ? "1-0"
        : winner === "black"
          ? "0-1"
          : winner === "draw"
            ? "1/2-1/2"
            : "*",
    );
    navigator.clipboard.writeText(game.pgn());
  };

  const getCurrentEvaluation = () => {
    if (!engineInitialized) return null;
    try {
      chessEngine.loadFEN(game.fen());
      return chessEngine.getEvaluation();
    } catch {
      return null;
    }
  };

  const pieceLabels: Record<string, string> = {
    wP: "white pawn",
    wN: "white Knight",
    wB: "white bishop",
    wR: "white rook",
    wQ: "white queen",
    wK: "white king",
    bP: "black pawn",
    bN: "black Knight",
    bB: "black bishop",
    bR: "black rook",
    bQ: "black queen",
    bK: "black king",
  };

  const customPieces = useMemo(() => {
    const pieces: any = {};

    Object.entries(pieceLabels).forEach(([notation, label]) => {
      pieces[notation] = ({ squareWidth }: { squareWidth: number }) => (
        <div
          role="img"
          aria-label={label}
          style={{ width: squareWidth, height: squareWidth }}
        >
          {defaultPieces[notation as keyof typeof defaultPieces]?.({
            squareWidth,
          })}
        </div>
      );
    });

    return pieces;
  }, []);

  const boardOptions = {
    boardOrientation,
    onPieceDrop,
    position,
    allowDragging: isStarted,
    allowDragOffBoard: false,
    animationDurationInMs: 300,
    canDragPiece: ({ piece }: PieceHandlerArgs) => {
      return piece.pieceType[0] === boardOrientation[0] && !playRandom;
    },
    darkSquareStyle: { backgroundColor: "#7B98B3" },
    draggingPieceGhostStyle: { opacity: 0.0 },
    draggingPieceStyle: { transform: "scale(1.0)" },
    dropSquareStyle: { boxShadow: "inset 0px 0px 0px 2px white" },
    lightSquareStyle: { backgroundColor: "#F0EAD6" },
    showAnimations: true,
    squareStyle: {
      borderRadius: "4px",
    },
    pieces: customPieces,
  };

  const onPromotionPieceSelect = (piece: PieceSymbol) => {
    try {
      game.move({
        from: promotionMove!.sourceSquare,
        to: promotionMove!.targetSquare as Square,
        promotion: piece,
      });
      setRedoStack([]);
      setPositionWrapper(game.fen());
    } catch {}
    setPromotionMove(null);
  };

  const squareWidth =
    document
      .querySelector(`[data-column="a"][data-row="1"]`)
      ?.getBoundingClientRect()?.width ?? 0;
  const promotionSquareLeft = promotionMove?.targetSquare
    ? squareWidth *
      chessColumnToColumnIndex(
        promotionMove.targetSquare.match(/^[a-z]+/)?.[0] ?? "",
        8,
        game.turn(),
      )
    : 0;

  return (
    <div className="container montserrat-base">
      <header>
        <h1
          onClick={() => setIsAboutPopUpOpen(true)}
          style={{ cursor: "pointer" }}
          title="Click for about"
        >
          FeohZero Engine
          <span className="info-icon">
            <Info />
          </span>
        </h1>
      </header>

      <main>
        <TurnIndicator
          winner={winner}
          turn={game.turn() == "w" ? "white" : "black"}
        />

        <div className="main-board">
          <div
            style={{
              display: "flex",
              flexDirection: "column",
              gap: "1rem",
              alignItems: "center",
            }}
          >
            {promotionMove ? (
              <div
                onClick={() => setPromotionMove(null)}
                onContextMenu={(e) => {
                  e.preventDefault();
                  setPromotionMove(null);
                }}
                style={{
                  position: "absolute",
                  top: 0,
                  left: 0,
                  right: 0,
                  bottom: 0,
                  backgroundColor: "rgba(0, 0, 0, 0.1)",
                  zIndex: 1000,
                }}
              />
            ) : null}

            {promotionMove ? (
              <div
                style={{
                  position: "absolute",
                  top: 0,
                  left: promotionSquareLeft,
                  backgroundColor: "white",
                  width: squareWidth,
                  zIndex: 1001,
                  display: "flex",
                  flexDirection: "column",
                  boxShadow: "0 0 10px 0 rgba(0, 0, 0, 0.5)",
                }}
              >
                {(["q", "r", "n", "b"] as const).map((piece) => {
                  const pieceColor = game.turn();
                  const pieceKey =
                    `${pieceColor}${piece.toUpperCase()}` as keyof typeof defaultPieces;

                  return (
                    <button
                      key={piece}
                      onClick={() => onPromotionPieceSelect(piece)}
                      onContextMenu={(e) => {
                        e.preventDefault();
                        setPromotionMove(null);
                      }}
                      style={{
                        width: "100%",
                        aspectRatio: "1",
                        display: "flex",
                        alignItems: "center",
                        justifyContent: "center",
                        padding: 0,
                        border: "none",
                        cursor: "pointer",
                        backgroundColor: "transparent",
                      }}
                    >
                      {defaultPieces[pieceKey]?.({ squareWidth })}
                    </button>
                  );
                })}
              </div>
            ) : null}

            <Chessboard options={boardOptions} />
          </div>
        </div>

        {engineInitialized && (
          <div className="search-info">
            <div className="search-info-content">
              {getCurrentEvaluation() !== null && (
                <>
                  <span>
                    eval:{" "}
                    {(getCurrentEvaluation() / 100)
                      .toFixed(2)
                      .padStart(6, "\u00A0")}
                  </span>
                  <span className="sep">•</span>
                </>
              )}
              <span>depth: {searchInfo.depth ?? "-"}</span>
              <span className="sep">•</span>
              <span>nodes: {searchInfo.nodes?.toLocaleString() ?? "-"}</span>
              <span className="sep">•</span>
              <span>
                time: {searchInfo.time ? `${searchInfo.time}ms` : "-"}
              </span>
              <span className="sep">•</span>
              <span>nps: {searchInfo.nps?.toLocaleString() ?? "-"}</span>
            </div>
          </div>
        )}

        <div className="buttons-container">
          <button
            onClick={boardFlip}
            onPointerUp={(e) => e.currentTarget.blur()}
            title="Flip the board"
          >
            <Flip />
          </button>
          <button onClick={boardReset} title="Reset the game">
            <Reset />
          </button>
          <button
            onClick={boardFirst}
            onPointerUp={(e) => e.currentTarget.blur()}
            className={game.history().length <= 0 ? "disabled" : ""}
            disabled={game.history().length <= 0}
            title="Go to the first move"
          >
            <First />
          </button>
          <button
            onClick={boardUndo}
            onPointerUp={(e) => e.currentTarget.blur()}
            className={game.history().length <= 0 ? "disabled" : ""}
            disabled={game.history().length <= 0}
            title="Undo the last move"
          >
            <Previous />
          </button>
          <button
            onClick={boardTogglePlayPause}
            onPointerUp={(e) => e.currentTarget.blur()}
            className={winner ? "disabled" : isStarted ? "active" : ""}
            disabled={winner !== null}
            title={isStarted ? "Pause the game" : "Start the game"}
          >
            {isStarted ? <Pause /> : <Play />}
          </button>
          <button
            onClick={boardRedo}
            onPointerUp={(e) => e.currentTarget.blur()}
            className={redoStack.length <= 0 ? "disabled" : ""}
            disabled={redoStack.length <= 0}
            title="Redo the last undone move"
          >
            <Next />
          </button>
          <button
            onClick={boardLast}
            onPointerUp={(e) => e.currentTarget.blur()}
            className={redoStack.length <= 0 ? "disabled" : ""}
            disabled={redoStack.length <= 0}
            title="Go to the last move"
          >
            <Last />
          </button>
          <button
            onClick={boardTogglePlayRandom}
            onPointerUp={(e) => e.currentTarget.blur()}
            className={playRandom ? "active" : ""}
            title="Play as random engine"
          >
            <Random />
          </button>

          <button
            onClick={() => setIsSettingsPopupOpen(true)}
            onPointerUp={(e) => e.currentTarget.blur()}
            title="Toggle settings menu"
          >
            <Settings />
          </button>
        </div>

        <div className="buttons-container-sec">
          <label htmlFor="fen-input">FEN</label>
          <input
            id="fen-input"
            onChange={() => {
              return;
            }}
            spellCheck="false"
            type="text"
            value={position}
          />
          <button
            onClick={boardExportFEN}
            onPointerUp={(e) => e.currentTarget.blur()}
            className="copy"
            title="Copy the current FEN position to clipboard"
          >
            <Copy />
          </button>
          <button
            onClick={() => {
              setIsFENPopupOpen(true);
            }}
            onPointerUp={(e) => e.currentTarget.blur()}
            className="set"
            title="Set a custom FEN position"
          >
            <Set />
          </button>
          &nbsp;&nbsp;&nbsp;
          <label htmlFor="pgn-export">PGN</label>
          <button
            id="pgn-export"
            onClick={boardExportPGN}
            onPointerUp={(e) => e.currentTarget.blur()}
            className="export"
            title="Export the game as PGN"
          >
            <Export />
          </button>
        </div>
      </main>

      <footer>
        <div className="credits">
          <a href="https://github.com/ohgreg/feohzero" target="_blank">
            Developed by ohgreg <Github />
          </a>
        </div>
      </footer>

      <AboutPopUp
        isOpen={isAboutPopUpOpen}
        onClose={() => setIsAboutPopUpOpen(false)}
      />
      <SettingsPopUp
        isOpen={isSettingsPopupOpen}
        onClose={() => setIsSettingsPopupOpen(false)}
        settings={engineSettings}
        onSubmit={boardHandleSettingsSubmit}
      />
      <FENPopup
        isOpen={isFENPopupOpen}
        onClose={() => setIsFENPopupOpen(false)}
        onSubmit={boardHandleFENSubmit}
      />
    </div>
  );
}

export default App;
