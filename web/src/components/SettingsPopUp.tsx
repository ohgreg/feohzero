import { useState, useEffect } from "react";

interface SettingsPopUpProps {
  isOpen: boolean;
  onClose: () => void;
  onSubmit: (settings: {
    depth: number;
    timeLimit: number;
    ttSize: number;
  }) => void;
  settings: {
    depth: number;
    timeLimit: number;
    ttSize: number;
  };
}

const SettingsPopUp = ({
  isOpen,
  onClose,
  onSubmit,
  settings,
}: SettingsPopUpProps) => {
  const [depth, setDepth] = useState<string>(settings.depth.toString());
  const [timeLimit, setTimeLimit] = useState<string>(
    settings.timeLimit.toString(),
  );
  const [ttSize, setTtSize] = useState<string>(settings.ttSize.toString());

  const [depthError, setDepthError] = useState<boolean>(false);
  const [timeLimitError, setTimeLimitError] = useState<boolean>(false);
  const [ttSizeError, setTtSizeError] = useState<boolean>(false);

  useEffect(() => {
    if (isOpen) {
      setDepth(settings.depth.toString());
      setTimeLimit(settings.timeLimit.toString());
      setTtSize(settings.ttSize.toString());
      setDepthError(false);
      setTimeLimitError(false);
      setTtSizeError(false);
    }
  }, [isOpen, settings]);

  if (!isOpen) return null;

  const handleSubmit = () => {
    const parsedDepth = parseInt(depth);
    const parsedTimeLimit = parseInt(timeLimit);
    const parsedTtSize = parseInt(ttSize);

    let hasError = false;

    if (isNaN(parsedDepth) || parsedDepth < 1 || parsedDepth > 50) {
      setDepthError(true);
      hasError = true;
    } else {
      setDepthError(false);
    }

    if (
      isNaN(parsedTimeLimit) ||
      parsedTimeLimit < 100 ||
      parsedTimeLimit > 30000
    ) {
      setTimeLimitError(true);
      hasError = true;
    } else {
      setTimeLimitError(false);
    }

    if (isNaN(parsedTtSize) || parsedTtSize < 0 || parsedTtSize > 65536) {
      setTtSizeError(true);
      hasError = true;
    } else {
      setTtSizeError(false);
    }

    if (hasError) {
      return;
    }

    onSubmit({
      depth: parsedDepth,
      timeLimit: parsedTimeLimit,
      ttSize: parsedTtSize,
    });
    onClose();
  };

  const handleCancel = () => {
    setDepth(settings.depth.toString());
    setTimeLimit(settings.timeLimit.toString());
    setTtSize(settings.ttSize.toString());
    setDepthError(false);
    setTimeLimitError(false);
    setTtSizeError(false);
    onClose();
  };

  const handleDepthChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const value = e.target.value;
    if (value === "" || /^\d+$/.test(value)) {
      setDepth(value);
      setDepthError(false);
    }
  };

  const handleTimeLimitChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const value = e.target.value;
    if (value === "" || /^\d+$/.test(value)) {
      setTimeLimit(value);
      setTimeLimitError(false);
    }
  };

  const handleTtSizeChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const value = e.target.value;
    if (value === "" || /^\d+$/.test(value)) {
      setTtSize(value);
      setTtSizeError(false);
    }
  };

  return (
    <div className="popup-overlay" onClick={handleCancel}>
      <div className="popup" onClick={(e) => e.stopPropagation()}>
        <h3>Engine settings</h3>
        <div
          style={{ paddingTop: "5px", marginBottom: "15px", textAlign: "left" }}
        >
          <label
            style={{ display: "block", marginBottom: "5px", fontSize: "14px" }}
          >
            Search depth (1 - 50)
          </label>
          <input
            type="text"
            value={depth}
            onChange={handleDepthChange}
            className={depthError ? "error" : ""}
          />
          <small
            style={{
              display: "block",
              color: "#808080",
              fontSize: "11px",
              marginTop: "3px",
            }}
          >
            maximum search depth to look for
          </small>
        </div>
        <div style={{ marginBottom: "15px", textAlign: "left" }}>
          <label
            style={{ display: "block", marginBottom: "5px", fontSize: "14px" }}
          >
            Time limit (100-30000 ms)
          </label>
          <input
            type="text"
            value={timeLimit}
            onChange={handleTimeLimitChange}
            className={timeLimitError ? "error" : ""}
          />
          <small
            style={{
              display: "block",
              color: "#808080",
              fontSize: "11px",
              marginTop: "3px",
            }}
          >
            maximum time per engine move
          </small>
        </div>
        <div style={{ marginBottom: "15px", textAlign: "left" }}>
          <label
            style={{ display: "block", marginBottom: "5px", fontSize: "14px" }}
          >
            TT size (0-65536 KB)
          </label>
          <input
            type="text"
            value={ttSize}
            onChange={handleTtSizeChange}
            className={ttSizeError ? "error" : ""}
          />
          <small
            style={{
              display: "block",
              color: "#808080",
              fontSize: "11px",
              marginTop: "3px",
            }}
          >
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
