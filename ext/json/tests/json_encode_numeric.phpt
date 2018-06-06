--TEST--
Test json_encode() function with numeric flag
--INI--
serialize_precision=-1
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php
var_dump(
	json_encode("1", JSON_NUMERIC_CHECK),
	json_encode("9.4324", JSON_NUMERIC_CHECK),
	json_encode(array("122321", "3232595.33423"), JSON_NUMERIC_CHECK),
	json_encode("1"),
	json_encode("9.4324"),
	json_encode(array("122321", "3232595.33423"))
);
?>
--EXPECT--
string(1) "1"
string(6) "9.4324"
string(22) "[122321,3232595.33423]"
string(3) ""1""
string(8) ""9.4324""
string(26) "["122321","3232595.33423"]"
