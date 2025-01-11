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
