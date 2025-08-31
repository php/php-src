--TEST--
Test deflate_init() error
--EXTENSIONS--
zlib
--FILE--
<?php

try {
    var_dump(deflate_init(42));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(deflate_init(ZLIB_ENCODING_DEFLATE, ['level' => 42]));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(deflate_init(ZLIB_ENCODING_DEFLATE, ['level' => -2]));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(deflate_init(ZLIB_ENCODING_DEFLATE, ['memory' => 0]));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(deflate_init(ZLIB_ENCODING_DEFLATE, ['memory' => 10]));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
deflate_init(): Argument #1 ($encoding) must be one of ZLIB_ENCODING_RAW, ZLIB_ENCODING_GZIP, or ZLIB_ENCODING_DEFLATE
deflate_init(): "level" option must be between -1 and 9
deflate_init(): "level" option must be between -1 and 9
deflate_init(): "memory" option must be between 1 and 9
deflate_init(): "memory" option must be between 1 and 9
