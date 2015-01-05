--TEST--
Bug #55543 (json_encode() with JSON_NUMERIC_CHECK & numeric string properties)
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php
$a = new stdClass;
$a->{"1"} = "5";

var_dump(json_encode($a, JSON_NUMERIC_CHECK));
?>
--EXPECT--
string(7) "{"1":5}"
