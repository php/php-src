--TEST--
Bug #79908 (json_encode encodes negative zero as int)
--SKIPIF--
<?php
if (!extension_loaded('json')) die("skip json extension not available");
?>
--FILE--
<?php
var_dump(json_encode(-0.));
?>
--EXPECT--
string(4) "-0.0"
