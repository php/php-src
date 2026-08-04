--TEST--
inflate_init(): window option type validation
--EXTENSIONS--
zlib
--FILE--
<?php

try {
    inflate_init(ZLIB_ENCODING_DEFLATE, ['window' => []]);
} catch (TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
inflate_init(): Argument #2 ($options) the value for option "window" must be of type int, array given
