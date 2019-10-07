# Cuteataxx
Cuteataxx runs computer engine matches for the board game [Ataxx](https://en.wikipedia.org/wiki/Ataxx). Still a work in progress.

## Usage
Match settings are passed to cuteataxx through command line arguments.
```
./cuteataxx --engine [name] [path] --engine [name] [path] --games=100 --concurrency=2
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
 - CLI11 is a command line parser and can be found [here](https://github.com/CLIUtils/CLI11). The header only version of this library is included in the libs folder.
