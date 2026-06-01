--TEST--
GH-22142 (Assertion failure in zendi_try_get_long() on IS_UNDEF)
--EXTENSIONS--
zlib
--FILE--
<?php

class Options {
    public int $level;
}

try {
    deflate_init(ZLIB_ENCODING_DEFLATE, new Options());
} catch (TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
deflate_init(): Argument #2 ($options) the value for option "level" must be of type int, null given
