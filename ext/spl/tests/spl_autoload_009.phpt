--TEST--
SPL: spl_autoload() and friends
--INI--
include_path=.
--FILE--
<?php

function my_autoload($name)
{
	require $name . '.class.inc';
	var_dump(class_exists($name));
}

spl_autoload_register("spl_autoload");
spl_autoload_register("my_autoload");

$obj = new testclass;

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
%stestclass.inc
%stestclass.class.inc
bool(true)
===DONE===
