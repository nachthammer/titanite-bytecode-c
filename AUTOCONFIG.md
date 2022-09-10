# Autoconfig

To make a good experience for every Linux user. We have a configuration via autoconfig.

1. Install autoconfig

```sh
sudo apt install autoconf
```

2. Generate the config file and a few helper files (which all should be ignored and not be committed.)

```sh
autoreconf -i
```

3. Generate the Makefile via the previously generated configure script

```sh
./configure
```

4. To everything you want to do with `make`.

```sh
make # to generate the binary
sudo make install # install dependencies if there are any (right now there are not)
make clean # to clean all the builded object files and binaries
```

