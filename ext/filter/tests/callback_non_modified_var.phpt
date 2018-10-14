--TEST--
callback function returns non modified value
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
function callback($var) {
	return $var;
}
$var = "test";
var_dump(filter_var($var, FILTER_CALLBACK, array('options'=>'callback')));
?>
--EXPECT--
string(4) "test"
