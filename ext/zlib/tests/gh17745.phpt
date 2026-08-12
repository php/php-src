--TEST--
GH-17745 (zlib extension incorrectly handles object arguments)
--EXTENSIONS--
zlib
--FILE--
<?php
$obj = new stdClass;
$obj->level = 3;
var_dump(deflate_init(ZLIB_ENCODING_RAW, $obj));

class Options {
    public int $level = 3;
}
var_dump(deflate_init(ZLIB_ENCODING_RAW, new Options));
?>
--EXPECTF--
Deprecated: deflate_init(): Passing an object for argument #2 $option to deflate_init() is deprecated, call get_object_vars() first instead in %s on line %d
object(DeflateContext)#2 (0) {
}

Deprecated: deflate_init(): Passing an object for argument #2 $option to deflate_init() is deprecated, call get_object_vars() first instead in %s on line %d
object(DeflateContext)#3 (0) {
}
