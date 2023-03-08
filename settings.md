# Settings
Match settings are provided in the [JSON](https://en.wikipedia.org/wiki/JSON) file format. An example of which can be found in the `res` directory [here](./res/settings.json).

---

# General

### __games__
The number of games to play per engine pair.

### __concurrency__
The number of games to play simultaneously.

### __ratinginterval__
How often to print updates.

### __verbose__
Print extra information about the match.

### __debug__
Enable debug to print engine communication.

### __recover__ (not implemented)
Continue the match in the event of an engine crash.

### __colour1__
The colour of player 1 in the .pgn file.

### __colour2__
The colour of player 2 in the .pgn file.

### __tournament__
The type of tournament to play: roundrobin, gauntlet

### __print_early__
Whether to print the results before the rating interval.

---

# Time control
Specifying how long the engines should spend thinking during a game.

### __timecontrol:movetime__
The number of milliseconds to spend per move.

### __timecontrol:time__
The number of milliseconds initially on each engine's clock.

### __timecontrol:increment__
The number of milliseconds added to the clock after each move is made.

---

# Adjudication
Ending games early based on certain rules.

### __adjudicate:gamelength__
The maximimum game length before being ruled a draw.

### __adjudicate:material__
The material advantage required for the game to be ruled a win.

### __adjudicate:easyfill__
Award a victory if the opponent is forced to pass while you can fill the rest of the empty squares.

### __adjudicate:timeout_buffer__
How far past the specified `movetime` an engine can think before losing on time.<br>
This setting does nothing for `time + increment` matches.

---

# Engines
Where to find and what to call engines, as well as what settings they need.

### __engines:arguments__
Command line arguments to be passed to the engine.

### __engines:protocol__
- UAI -- the only protocol engines should use based on UCI from chess.
- FSF -- supported exclusively for the sake of Fairy-Stockfish found [here](https://github.com/ianfab/Fairy-Stockfish).
- KataGo -- partial support exclusively for a KataGo fork found [here](https://github.com/hzyhhzy/KataGo/tree/Ataxx).

### __engines:builtin__
This command lets you choose from a limited selection of built in engines. They are mostly intended as opponents for weak engines to test correctness.
- random -- play a random legal move.
- mostcaptures -- play the move that maximises `num_captures + is_single`.
- leastcaptures -- play the move that minimises `num_captures + is_single`.
