--TEST--
Bug #39003 (__autoload() is called for type hinting)
--FILE--
<?php

class ClassName
{
	public $var = 'bla';
}

function test (OtherClassName $object) { }

function __autoload($class)
{
    var_dump("__autload($class)");
}

$obj = new ClassName;
test($obj);

echo "Done\n";
?>
--EXPECTF--	
Catchable fatal error: Argument 1 passed to test() must be an instance of OtherClassName, instance of ClassName given, called in %s on line %d and defined in %s on line %d
