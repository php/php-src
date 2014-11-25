--TEST--
Check if final abstract classes are final indeed
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

class B extends A
{
}

?>
--EXPECTF--
Fatal error: Class %s may not inherit from final class (%s) in %s on line %d