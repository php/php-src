--TEST--
Check if final abstract classes prevents abstract static methods
--SKIPIF--
<?php if (version_compare(zend_version(), '2.4.0', '<=')) die('skip ZendEngine 2.4 or later needed'); ?>
--FILE--
<?php

final abstract class A
{
	public static abstract function display($name);
}

?>
--EXPECTF--
Fatal error: Class %s contains abstract static method (%s) and therefore cannot be declared 'abstract final' in %s on line %d