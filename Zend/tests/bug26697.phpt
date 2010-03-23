--TEST--
Bug #26697 (calling class_exists on a nonexistent class in __autoload results in segfault)
--SKIPIF--
<?php if (function_exists('__autoload')) die('skip __autoload() declared in auto_prepend_file');?>
--FILE--
<?php

function __autoload($name)
{
	echo __METHOD__ . "($name)\n";
	var_dump(class_exists('NotExistingClass'));
	echo __METHOD__ . "($name), done\n";
}

var_dump(class_exists('NotExistingClass'));

?>
===DONE===
--EXPECTF--
__autoload(NotExistingClass)
bool(false)
__autoload(NotExistingClass), done
bool(false)
===DONE===
