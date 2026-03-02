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
--EXPECT--
object(DeflateContext)#2 (0) {
}
object(DeflateContext)#3 (0) {
}
