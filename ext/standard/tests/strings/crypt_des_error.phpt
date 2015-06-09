--TEST--
crypt(): *0 should return *1
--SKIPIF--
<?php
if (!function_exists('crypt')) {
	die("SKIP crypt() is not available");
}
?>
--FILE--
<?php

var_dump(crypt('foo', '*0'));

?>
--EXPECT--
string(2) "*1"
