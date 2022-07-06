Fuzzing SAPI for PHP
--------------------

The following `./configure` options can be used to enable the fuzzing SAPI, as well as all availablefuzzers. If you don't build the exif/json/mbstring extensions, fuzzers for these extensions will not be built.

```sh
CC=clang CXX=clang++ \
./configure \
    --disable-all \
    --enable-fuzzer \
    --with-pic \
    --enable-debug-assertions \
    --enable-exif \
    --enable-mbstring
```

The `--with-pic` option is required to avoid a linking failure. The `--enable-debug-assertions` option can be used to enable debug assertions despite the use of a release build.

You will need a recent version of clang that supports the `-fsanitize=fuzzer-no-link` option.

When running `make` it creates these binaries in `sapi/fuzzer/`:

* `php-fuzz-parser`: Fuzzing language parser and compiler
* `php-fuzz-unserialize`: Fuzzing unserialize() function
* `php-fuzz-unserializehash`: Fuzzing unserialize() for HashContext objects
* `php-fuzz-json`: Fuzzing JSON parser (requires --enable-json)
* `php-fuzz-exif`: Fuzzing `exif_read_data()` function (requires --enable-exif)
* `php-fuzz-mbstring`: Fuzzing `mb_ereg[i]()` (requires --enable-mbstring)
* `php-fuzz-execute`: Fuzzing the executor

Some fuzzers have a seed corpus in `sapi/fuzzer/corpus`. You can use it as follows:

```sh
cp -r sapi/fuzzer/corpus/exif ./my-exif-corpus
sapi/fuzzer/php-fuzz-exif ./my-exif-corpus
```

For the unserialize fuzzer, a dictionary of internal classes should be generated first:

```sh
sapi/cli/php sapi/fuzzer/generate_unserialize_dict.php
cp -r sapi/fuzzer/corpus/unserialize ./my-unserialize-corpus
sapi/fuzzer/php-fuzz-unserialize -dict=$PWD/sapi/fuzzer/dict/unserialize ./my-unserialize-corpus
```

For the unserializehash fuzzer, generate a corpus of initial hash serializations:

```sh
sapi/cli/php sapi/fuzzer/generate_unserializehash_corpus.php
cp -r sapi/fuzzer/corpus/unserializehash ./my-unserialize-corpus
sapi/fuzzer/php-fuzz-unserializehash ./my-unserialize-corpus
```

For the parser fuzzer, a corpus may be generated from Zend test files:

```sh
sapi/cli/php sapi/fuzzer/generate_parser_corpus.php
mkdir ./my-parser-corpus
sapi/fuzzer/php-fuzz-parser -merge=1 ./my-parser-corpus sapi/fuzzer/corpus/parser
sapi/fuzzer/php-fuzz-parser -only_ascii=1 ./my-parser-corpus
```

For the mbstring fuzzer, you may want to build the libonig dependency with instrumentation. At this time, libonig is not clean under ubsan, so only the fuzzer and address sanitizers may be used.

```sh
git clone https://github.com/kkos/oniguruma.git
pushd oniguruma
autoreconf -vfi
./configure CC=clang CFLAGS="-fsanitize=fuzzer-no-link,address -O2 -g"
make
popd

export ONIG_CFLAGS="-I$PWD/oniguruma/src"
export ONIG_LIBS="-L$PWD/oniguruma/src/.libs -l:libonig.a"
```

This will link an instrumented libonig statically into the PHP binary.
