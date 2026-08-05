--TEST--
GH-22142 (Assertion failure in zendi_try_get_long() on IS_UNDEF)
--EXTENSIONS--
zlib
--FILE--
<?php

class Options {
    public int $window;
}

try {
    inflate_init(ZLIB_ENCODING_DEFLATE, new Options());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
Deprecated: inflate_init(): Passing an object for argument #2 $option to inflate_init() is deprecated, call get_object_vars() first instead in %s on line %d
TypeError: inflate_init(): Argument #2 ($options) the value for option "window" must be of type int, null given
