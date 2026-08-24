--TEST--
GH-22142 (Assertion failure in zendi_try_get_long() on IS_UNDEF)
--CREDITS--
JIANG Yuancheng
--EXTENSIONS--
zlib
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php

class DeflateOptions {
    public int $level;
    public int $memory;
    public int $window;
    public int $strategy;
    public string $dictionary;
}

class InflateOptions {
    public int $window;
    public string $dictionary;
}

class BadDeflateOptions {
    public int $level = 42;
    public int $memory;
}

$deflate = new DeflateOptions();
var_dump(deflate_init(ZLIB_ENCODING_DEFLATE, $deflate) instanceof DeflateContext);
var_dump(deflate_init(ZLIB_ENCODING_DEFLATE, get_object_vars($deflate)) instanceof DeflateContext);

$inflate = new InflateOptions();
var_dump(inflate_init(ZLIB_ENCODING_DEFLATE, $inflate) instanceof InflateContext);
var_dump(inflate_init(ZLIB_ENCODING_DEFLATE, get_object_vars($inflate)) instanceof InflateContext);

try {
    deflate_init(ZLIB_ENCODING_DEFLATE, new BadDeflateOptions());
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
ValueError: deflate_init(): "level" option must be between -1 and 9
