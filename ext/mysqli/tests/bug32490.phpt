--TEST--
Bug #32490 (constructor of mysqli has wrong name)
--SKIPIF--
<?php if (!extension_loaded("mysqli")) die("skip mysqli not available"); ?>
--FILE--
<?php
var_dump(method_exists("mysqli", "mysqli"));
var_dump(method_exists("mysqli", "__construct"));
?>
--EXPECT--
bool(false)
bool(true)

