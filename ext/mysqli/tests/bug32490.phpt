--TEST--
Bug #32490 (constructor of mysqli has wrong name)
--EXTENSIONS--
mysqli
--FILE--
<?php
var_dump(method_exists("mysqli", "mysqli"));
var_dump(method_exists("mysqli", "__construct"));
?>
--EXPECT--
bool(false)
bool(true)
