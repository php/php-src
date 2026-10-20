--TEST--
GH-16326 (Memory management is broken for bad dictionaries)
--EXTENSIONS--
zlib
--FILE--
<?php
try {
    deflate_init(ZLIB_ENCODING_DEFLATE, ["dictionary" => [" ", ""]]);
} catch (ValueError $ex) {
    echo $ex::class, ': ', $ex->getMessage(), "\n";
}
try {
    deflate_init(ZLIB_ENCODING_DEFLATE, ["dictionary" => ["hello", "wor\0ld"]]);
} catch (ValueError $ex) {
    echo $ex::class, ': ', $ex->getMessage(), "\n";
}
try {
    deflate_init(ZLIB_ENCODING_DEFLATE, ["dictionary" => [" ", new stdClass]]);
} catch (Error $ex) {
    echo $ex::class, ': ', $ex->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: deflate_init(): Argument #2 ($options) must not contain empty strings
ValueError: deflate_init(): Argument #2 ($options) must not contain strings with null bytes
Error: Object of class stdClass could not be converted to string
