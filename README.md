# Cuteataxx
Cuteataxx runs computer engine matches for the board game [Ataxx](https://en.wikipedia.org/wiki/Ataxx). Still a work in progress.

---

# Usage
Match settings are read from a .json file passed to cuteataxx through command line arguments
```
./cuteataxx settings.json
```

---

# Building
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

# Settings

### __games__
The number of games to play in the match

### __concurrency__
The number of games to play simultaneously

### __ratinginterval__
How often to print updates

### __verbose__
Print extra information about the match

### __debug__
Enable debug to print engine communication

### __recover__ (not implemented)
Continue the match in the event of an engine crash

### __colour1__
The colour of player 1 in the .pgn file

### __colour2__
The colour of player 2 in the .pgn file

### __adjudicate:gamelength__
The maximimum game length before being ruled a draw

### __adjudicate:material__
The material advantage required for the game to be ruled a win

### __adjudicate:easyfill__
Award a victory if the opponent is forced to pass while you can fill the rest of the empty squares

### __engines:protocol__
- UAI -- the only protocol engines should use based on UCI from chess.
- FSF -- supported exclusively for the sake of Fairy-Stockfish found [here](https://github.com/ianfab/Fairy-Stockfish).
- KataGo -- partial support exclusively for a KataGo fork found [here](https://github.com/hzyhhzy/KataGo/tree/Ataxx).

---

# License
Cuteataxx is available under the MIT license.

---

# Thanks
- [JSON for Modern C++](https://github.com/nlohmann/json) JSON for Modern C++
- To everyone partaking in Ataxx engine programming
