import { useState } from "react";

interface FENPopupProps {
  isOpen: boolean;
  onClose: () => void;
  onSubmit: (fen: string) => void;
}

const FENPopup = ({ isOpen, onClose, onSubmit }: FENPopupProps) => {
  const [fenInput, setFenInput] = useState<string>("");

  if (!isOpen) return null;

  const handleSubmit = () => {
    onSubmit(fenInput);
    setFenInput("");
  };

  const handleCancel = () => {
    setFenInput("");
    onClose();
  };

  return (
    <div className="popup-overlay" onClick={onClose}>
      <div className="popup" onClick={(e) => e.stopPropagation()}>
        <h3>Set FEN</h3>
        <input
          spellCheck="false"
          type="text"
          value={fenInput}
          onChange={(e) => setFenInput(e.target.value)}
          placeholder="Enter FEN..."
        />
        <div className="popup-buttons">
          <button onClick={handleCancel}>Cancel</button>
          <button onClick={handleSubmit}>Set</button>
        </div>
      </div>
    </div>
  );
};

export default FENPopup;
