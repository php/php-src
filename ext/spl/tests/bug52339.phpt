--TEST--
Bug #52339: SPL autoloader breaks class_exists()
--FILE--
<?php
var_dump(class_exists('asdfasdf'));
spl_autoload_register();
var_dump(class_exists('asdfasdf'));
?>
--EXPECT--
bool(false)
bool(false)
