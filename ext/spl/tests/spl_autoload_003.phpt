--TEST--
SPL: spl_autoload() and friends
--INI--
include_path=.
--FILE--
<?php

function TestFunc1($classname)
{
	echo __METHOD__ . "($classname)\n";
}

function TestFunc2($classname)
{
	echo __METHOD__ . "($classname)\n";
	throw new Exception("Class $classname missing");
}

function TestFunc3($classname)
{
	echo __METHOD__ . "($classname)\n";
}

spl_autoload_register("TestFunc1");
spl_autoload_register("TestFunc2");
spl_autoload_register("TestFunc3");

try
{
	var_dump(class_exists("TestClass", true));
}
catch(Exception $e)
{
	echo 'Exception: ' . $e->getMessage() . "\n";
}

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
TestFunc1(TestClass)
TestFunc2(TestClass)
TestFunc3(TestClass)
Exception: Class TestClass missing
===DONE===
