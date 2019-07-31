Fuzzing SAPI for PHP

Enable fuzzing targets with --enable-fuzzer switch.

Your compiler should support -fsanitize=address and you need
to have Fuzzer library around.

When running `make` it creates these binaries in `sapi/fuzzer/`:
* php-fuzz-parser - fuzzing language parser
* php-fuzz-unserialize - fuzzing unserialize() function
* php-fuzz-json - fuzzing JSON parser
* php-fuzz-exif - fuzzing exif_read_data() function (use --enable-exif)
* php-fuzz-mbstring - fuzzing mb_ereg[i] (requires --enable-mbstring)
