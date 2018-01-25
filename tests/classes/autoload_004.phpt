--TEST--
ZE2 Autoload and recursion
--SKIPIF--
<?php
	if (class_exists('autoload_root', false)) die('skip Autoload test classes exist already');
?>
--FILE--
<?php

spl_autoload_register(function ($class_name) {
	var_dump(class_exists($class_name));
	require_once(dirname(__FILE__) . '/' . $class_name . '.p5c');
	echo 'autoload(' . $class_name . ")\n";
});

var_dump(class_exists('autoload_derived'));

?>
===DONE===
--EXPECT--
bool(false)
bool(false)
autoload(autoload_root)
autoload(autoload_derived)
bool(true)
===DONE===
