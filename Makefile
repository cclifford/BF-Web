CC = emcc
CFLAGS = -s EXPORTED_FUNCTIONS='["_run"]' -s EXPORTED_RUNTIME_METHODS='["cwrap", "ccall"]'

default:
	$(CC) $(CFLAGS)  -o bf.js bf.cpp

clean:
	rm bf.wasm bf.js

serve: default
	python3 -m http.server
