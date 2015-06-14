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
Fatal error: Uncaught TypeError: Argument 1 passed to test() must be an instance of OtherClassName, instance of ClassName given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(%d): test(Object(ClassName))
#1 {main}
  thrown in %s on line %d
