--TEST--
Check if final abstract classes enforces static properties
--SKIPIF--
<?php if (version_compare(zend_version(), '2.4.0', '<=')) die('skip ZendEngine 2.4 or later needed'); ?>
--FILE--
<?php

final abstract class A
{
	public $foo = 'bar';
}

?>
--EXPECTF--
Fatal error: Class %s contains non-static property declaration and therefore cannot be declared 'abstract final' in %s on line %d