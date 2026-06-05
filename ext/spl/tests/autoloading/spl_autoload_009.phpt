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

$obj = new TestClass;

?>
--EXPECTF--
%sTestClass.inc
%sTestClass.class.inc
bool(true)
