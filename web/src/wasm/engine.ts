import wasmUrl from "../wasm/feohzero.wasm?url";
import glueUrl from "../wasm/feohzero.js?url";

/* emscripten module interface */
type ChessEngineModule = {
  _wasm_init: (ttSizeKb: number) => void;
  _wasm_clear: () => void;
  _wasm_load_fen: (fenPtr: number) => number;
  _wasm_search: (depth: number, timeoutMs: number) => number;
  _wasm_get_depth: () => number;
  _wasm_get_nodes: () => number;
  _wasm_get_time: () => number;
  _wasm_get_nps: () => number;
  _wasm_is_timeout: () => number;
  _wasm_is_book_move: () => number;
  _wasm_eval: () => number;
  ccall: (
    name: string,
    returnType: string | null,
    argTypes: string[],
    args: any[],
  ) => any;
  cwrap: (
    name: string,
    returnType: string | null,
    argTypes: string[],
  ) => Function;
  stringToNewUTF8: (str: string) => number;
  _malloc: (size: number) => number;
  _free: (ptr: number) => void;
  UTF8ToString: (ptr: number) => string;
};

/* search result */
interface SearchResult {
  move: string;
  depth: number;
  nodes: number;
  time: number;
  nps: number;
  timeout: boolean;
  bookMove: boolean;
}

/* chess engine wrapper for WASM module */
class ChessEngine {
  private module: ChessEngineModule | null = null;
  private initialized = false;

  /* initializes the WASM engine, given transposition table size */
  async initialize(ttSizeKb: number = 64): Promise<void> {
    if (this.initialized) return;
    try {
      const script = document.createElement("script");
      script.src = glueUrl;
      const scriptLoaded = new Promise((resolve, reject) => {
        script.onload = resolve;
        script.onerror = reject;
      });
      document.head.appendChild(script);
      await scriptLoaded;
      const createModule = (window as any).createChessEngine;
      if (!createModule) {
        throw new Error("WebAssembly failure");
      }
      this.module = await createModule({
        locateFile: (path: string) => {
          if (path.endsWith(".wasm")) return wasmUrl;
          return path;
        },
      });
      this.module!._wasm_init(ttSizeKb);
      this.initialized = true;
    } catch (error) {
      throw error;
    }
  }

  /* loads a FEN position, returns true on success */
  loadFEN(fen: string): boolean {
    if (!this.module) throw new Error("engine not initialized");
    const fenPtr = this.module.stringToNewUTF8(fen);
    const result = this.module._wasm_load_fen(fenPtr);
    this.module._free(fenPtr);
    return result === 1;
  }

  /* runs search and returns its result */
  search(depth: number = 5, timeoutMs: number = 5000): SearchResult {
    if (!this.module) throw new Error("engine not initialized");
    const movePtr = this.module._wasm_search(depth, timeoutMs);
    const move = this.module.UTF8ToString(movePtr);
    this.module._wasm_clear();
    return {
      move,
      depth: this.module._wasm_get_depth(),
      nodes: this.module._wasm_get_nodes(),
      time: this.module._wasm_get_time(),
      nps: this.module._wasm_get_nps(),
      timeout: this.module._wasm_is_timeout() === 1,
      bookMove: this.module._wasm_is_book_move() === 1,
    };
  }

  /* returns static evaluation of current position */
  getEvaluation(): number {
    if (!this.module) throw new Error("engine not initialized");
    return this.module._wasm_eval();
  }

  /* clears the transposition table */
  clearTT(): void {
    if (this.module) this.module._wasm_clear();
  }

  /* returns true if engine is initialized */
  isInitialized(): boolean {
    return this.initialized;
  }
}

export default new ChessEngine();
