--TEST--
json_encode() with JSON_FORCE_EMPTY_OBJECT
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php
var_dump(json_encode(array(), JSON_FORCE_EMPTY_OBJECT));
var_dump(json_encode(array(1,2,3), JSON_FORCE_EMPTY_OBJECT));
?>
--EXPECT--
string(2) "{}"
string(7) "[1,2,3]"
