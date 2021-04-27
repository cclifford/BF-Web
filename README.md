# BF-Web
Web-Based Brainf*** interpreter

Have you ever been unable to sleep because your brain won't turn off?
This can help. BF-Web is an interpreter for the esoteric programming 
language brainfuck which can run in any modern web browser (Supporting 
Webassembly). Not all that much to it. I like to give myself little puzzles 
to implement to help me to fall asleep.

 - what is the shortest way to generate a particular number?
 - How can I read in an ASCII code in decimal and print out the corresponding letter?

Things like that. Brainfuck makes all of that complex enough to distract me from my other thoughts.

# Compiling

BF-Web requires a working installation of [Emscripten](https://emscripten.org/) to compile. With the Emscripten SDK in your `$PATH`
just run `make` in the project directory. You can host the resulting files with any web server. If you have Python installed, 
you can quickly serve any directory with `python -m http.server` inside that directory. That is pretty much it!
