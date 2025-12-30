import { useState } from "react";

interface FENPopUpProps {
  isOpen: boolean;
  onClose: () => void;
  onSubmit: (fen: string) => boolean;
}

const FENPopUp = ({ isOpen, onClose, onSubmit }: FENPopUpProps) => {
  const [fenInput, setFenInput] = useState<string>("");
  const [fenError, setFenError] = useState<boolean>(false);

  if (!isOpen) return null;

  const handleSubmit = () => {
    const result = onSubmit(fenInput);
    if (result === false) {
      setFenError(true);
      return;
    }
    setFenInput("");
    setFenError(false);
  };

  const handleCancel = () => {
    setFenInput("");
    setFenError(false);
    onClose();
  };

  const handleChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    setFenInput(e.target.value);
    setFenError(false);
  };

  return (
    <div className="popup-overlay" onClick={handleCancel}>
      <div className="popup" onClick={(e) => e.stopPropagation()}>
        <h3>Set FEN</h3>
        <div className="input-container">
          <input
            spellCheck="false"
            type="text"
            value={fenInput}
            onChange={handleChange}
            placeholder="Enter FEN..."
            className={fenError ? 'error' : ''}
          />
          <small className={`error-message ${fenError ? 'visible' : ''}`}>
            {fenError ? 'Invalid FEN string. Please try again.' : '\u00A0'}
          </small>
        </div>
        <div className="popup-buttons">
          <button onClick={handleCancel}>Cancel</button>
          <button onClick={handleSubmit}>Set</button>
        </div>
      </div>
    </div>
  );
};

export default FENPopUp;
