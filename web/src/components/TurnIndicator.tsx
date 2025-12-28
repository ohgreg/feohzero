import React from "react";

interface TurnIndicatorProps {
  winner: "white" | "black" | "draw" | null;
  turn: "white" | "black";
}

const TurnIndicator: React.FC<TurnIndicatorProps> = ({ winner, turn }) => (
  <div className="turn-indicator">
    <span
      className={
        !winner
          ? turn === "white"
            ? "active"
            : ""
          : winner !== "white"
            ? "disabled"
            : ""
      }
    >
      White
    </span>
    <span
      className={
        !winner
          ? turn === "black"
            ? "active"
            : ""
          : winner !== "black"
            ? "disabled"
            : ""
      }
    >
      Black
    </span>
  </div>
);

export default TurnIndicator;
