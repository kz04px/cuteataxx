# Cuteataxx
Cuteataxx runs computer engine matches for the board game [Ataxx](https://en.wikipedia.org/wiki/Ataxx). Still a work in progress.

---

## Usage
Match settings are read from a .json file passed to cuteataxx through command line arguments
```
./cuteataxx settings.json
```

---

## Building
```
git clone --recurse-submodules https://github.com/kz04px/cuteataxx
cd ./cuteataxx
sh build-linux.sh
```
or
```
git clone --recurse-submodules https://github.com/kz04px/cuteataxx
follow libataxx build instructions
mkdir ./cuteataxx/build
cd ./cuteataxx/build
cmake ..
make
```

---

## Settings
```
> games
The number of games to play in the match

> concurrency
The number of games to play simultaneously

> ratinginterval
How often to print updates

> verbose
Print extra information about the match

> debug (not implemented)
Enable debug to print engine output

> recover (not implemented)
Continue the match in the event of an engine crash

> colour1
The colour of player 1 in the .pgn file

> colour2
The colour of player 2 in the .pgn file

> adjudicate:gamelength
The maximimum game length before being ruled a draw

> adjudicate:material
The material advantage required for the game to be ruled a win

> adjudicate:easyfill
Award a victory if the opponent is forced to pass while you can fill the rest of the empty squares

> engines:protocol
This has to be set to one of the following values: UAI, UCI
UAI is the only protocol engines should use.
UCI is supported exclusively for the sake of Fairy-Stockfish.
```

---

## License
Cuteataxx is available under the MIT license.

---

## Thanks
- [JSON for Modern C++](https://github.com/nlohmann/json)
- To everyone partaking in Ataxx engine programming
