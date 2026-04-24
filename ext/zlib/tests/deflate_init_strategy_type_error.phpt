--TEST--
deflate_init(): options type validation
--EXTENSIONS--
zlib
--FILE--
<?php

try {
    deflate_init(ZLIB_ENCODING_DEFLATE, ['level' => []]);
} catch (TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    deflate_init(ZLIB_ENCODING_DEFLATE, ['memory' => []]);
} catch (TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    deflate_init(ZLIB_ENCODING_DEFLATE, ['window' => []]);
} catch (TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    deflate_init(ZLIB_ENCODING_DEFLATE, ['strategy' => []]);
} catch (TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
deflate_init(): Argument #2 ($options) the value for option "level" must be of type int, array given
deflate_init(): Argument #2 ($options) the value for option "memory" must be of type int, array given
deflate_init(): Argument #2 ($options) the value for option "window" must be of type int, array given
deflate_init(): Argument #2 ($options) the value for option "strategy" must be of type int, array given
