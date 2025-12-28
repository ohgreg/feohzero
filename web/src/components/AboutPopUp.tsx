interface AboutPopUpProps {
  isOpen: boolean;
  onClose: () => void;
}

const AboutPopUp = ({ isOpen, onClose }: AboutPopUpProps) => {
  if (!isOpen) return null;

  return (
    <div className="popup-overlay" onClick={onClose}>
      <div className="popup" onClick={(e) => e.stopPropagation()} style={{ maxWidth: "400px" }}>
        <h3>About the engine</h3>
        <div
          style={{ textAlign: "left", marginTop: "15px", lineHeight: "1.6" }}
        >
          <p>
            FeohZero is a chess engine written in C and compiled to WebAssembly for this interactive web app.
          </p>
          <p style={{ marginTop: "10px" }}>
            View the source code on{" "}
            <a
              href="https://github.com/ohgreg/feohzero"
              target="_blank"
              rel="noopener noreferrer"
              style={{ color: "#7b98b3", textDecoration: "underline" }}
            >
              GitHub
            </a>
            .
          </p>
          <p style={{ marginTop: "10px", fontSize: "12px", color: "#808080" }}>
            <strong>Note:</strong> this web version may not be as stable or fast as the native C version due to WebAssembly limitations and browser constraints. Performance may vary depen-ding on your browser and device.
          </p>
        </div>
        <div className="popup-buttons">
          <button onClick={onClose}>Close</button>
        </div>
      </div>
    </div>
  );
};

export default AboutPopUp;
