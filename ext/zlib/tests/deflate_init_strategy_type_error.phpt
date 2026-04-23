--TEST--
deflate_init(): strategy option type validation
--EXTENSIONS--
zlib
--FILE--
<?php

try {
    deflate_init(ZLIB_ENCODING_DEFLATE, ['strategy' => []]);
} catch (TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
deflate_init(): Argument #2 ($options) "strategy" option must be of type int, array given
