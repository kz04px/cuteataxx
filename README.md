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
git clone https://github.com/kz04px/cuteataxx
mkdir ./cuteataxx/build
cd ./cuteataxx/build
cmake ..
cmake --build .
```

---

# Settings
Match settings are provided in the [JSON](https://en.wikipedia.org/wiki/JSON) file format. An example of which can be found in the `res` directory [here](./res/settings.json). Details of the settings available can be found [here](./settings.md).

---

# License
Cuteataxx is available under the MIT license.

---

# Thanks
- [JSON for Modern C++](https://github.com/nlohmann/json) JSON for Modern C++
- [doctest](https://github.com/doctest/doctest) The fastest feature-rich C++11/14/17/20 single-header testing framework
- [FTXUI](https://github.com/ArthurSonzogni/FTXUI) C++ Functional Terminal User Interface.
- To everyone partaking in Ataxx engine programming
