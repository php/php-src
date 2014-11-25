--TEST--
Check if final abstract classes are supported
--SKIPIF--
<?php if (version_compare(zend_version(), '2.4.0', '<=')) die('skip ZendEngine 2.4 or later needed'); ?>
--FILE--
<?php

final abstract class A
{
	public static function display($name)
	{
        echo "Hello $name\n";
    }
}

A::display('Foo');

$a = new A;

?>
--EXPECTF--
Hello Foo

Fatal error: Cannot instantiate abstract class %s in %s on line %d