--TEST--
Bug #79908 (json_decode decodes negative zero as positive zero)
--SKIPIF--
<?php
if (!extension_loaded('json')) die("skip json extension not available");
?>
--FILE--
<?php
var_dump(json_decode("-0"));
?>
--EXPECT--
float(-0)
