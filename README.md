# Cuteataxx
Cuteataxx runs computer engine matches for the board game [Ataxx](https://en.wikipedia.org/wiki/Ataxx). Still a work in progress.

## Usage
Match settings are read from a .json file passed to cuteataxx through command line arguments
```
./cuteataxx settings.json
```

## Building
```bash
git clone --recurse-submodules https://github.com/kz04px/cuteataxx
cd ./cuteataxx/libs/libataxx
make
cd ../../
make release
```

## License
Cuteataxx is available under the MIT license.

## Credits
 - [JSON for Modern C++](https://github.com/nlohmann/json)
