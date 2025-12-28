import { useState, useEffect } from "react";

interface SettingsPopUpProps {
  isOpen: boolean;
  onClose: () => void;
  onSubmit: (settings: { depth: number; timeLimit: number; ttSize: number }) => void;
  settings: {
    depth: number;
    timeLimit: number;
    ttSize: number;
  };
}

const SettingsPopUp = ({ isOpen, onClose, onSubmit, settings }: SettingsPopUpProps) => {
  const [depth, setDepth] = useState<string>(settings.depth.toString());
  const [timeLimit, setTimeLimit] = useState<string>(settings.timeLimit.toString());
  const [ttSize, setTtSize] = useState<string>(settings.ttSize.toString());

  useEffect(() => {
    if (isOpen) {
      setDepth(settings.depth.toString());
      setTimeLimit(settings.timeLimit.toString());
      setTtSize(settings.ttSize.toString());
    }
  }, [isOpen, settings]);

  if (!isOpen) return null;

  const handleSubmit = () => {
    const parsedDepth = parseInt(depth);
    const parsedTimeLimit = parseInt(timeLimit);
    const parsedTtSize = parseInt(ttSize);

    if (isNaN(parsedDepth) || parsedDepth < 1 || parsedDepth > 20) {
      alert("Search depth must be between 1 and 20.");
      return;
    }

    if (isNaN(parsedTimeLimit) || parsedTimeLimit < 100 || parsedTimeLimit > 30000) {
      alert("Time limit must be between 100 and 60000 ms.");
      return;
    }

    if (isNaN(parsedTtSize) || parsedTtSize < 0 || parsedTtSize > 65536) {
      alert("TT size must be between 0 and 65536 KB.");
      return;
    }

    onSubmit({
      depth: parsedDepth,
      timeLimit: parsedTimeLimit,
      ttSize: parsedTtSize
    });
    onClose();
  };

  const handleCancel = () => {
    setDepth(settings.depth.toString());
    setTimeLimit(settings.timeLimit.toString());
    setTtSize(settings.ttSize.toString());
    onClose();
  };

  const handleDepthChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const value = e.target.value;
    if (value === '' || /^\d+$/.test(value)) {
      setDepth(value);
    }
  };

  const handleTimeLimitChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const value = e.target.value;
    if (value === '' || /^\d+$/.test(value)) {
      setTimeLimit(value);
    }
  };

  const handleTtSizeChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const value = e.target.value;
    if (value === '' || /^\d+$/.test(value)) {
      setTtSize(value);
    }
  };

  return (
    <div className="popup-overlay" onClick={handleCancel}>
      <div className="popup" onClick={(e) => e.stopPropagation()}>
        <h3>Engine settings</h3>
        <div style={{ paddingTop: '5px', marginBottom: '15px', textAlign: 'left' }}>
          <label style={{ display: 'block', marginBottom: '5px', fontSize: '14px' }}>
            Search depth (1 - 50)
          </label>
          <input
            type="text"
            value={depth}
            onChange={handleDepthChange}
          />
          <small style={{ display: 'block', color: '#808080', fontSize: '11px', marginTop: '3px' }}>
            maximum search depth to look for
          </small>
        </div>
        <div style={{ marginBottom: '15px', textAlign: 'left' }}>
          <label style={{ display: 'block', marginBottom: '5px', fontSize: '14px' }}>
            Time limit (100-30000 ms)
          </label>
          <input
            type="text"
            value={timeLimit}
            onChange={handleTimeLimitChange}
          />
          <small style={{ display: 'block', color: '#808080', fontSize: '11px', marginTop: '3px' }}>
            maximum time per engine move
          </small>
        </div>
        <div style={{ marginBottom: '15px', textAlign: 'left' }}>
          <label style={{ display: 'block', marginBottom: '5px', fontSize: '14px' }}>
            TT size (0-65536 KB)
          </label>
          <input
            type="text"
            value={ttSize}
            onChange={handleTtSizeChange}
          />
          <small style={{ display: 'block', color: '#808080', fontSize: '11px', marginTop: '3px' }}>
            memory allocated for transposition table
          </small>
        </div>
        <div className="popup-buttons">
          <button onClick={handleCancel}>Cancel</button>
          <button onClick={handleSubmit}>Apply</button>
        </div>
      </div>
    </div>
  );
};

export default SettingsPopUp;
