--TEST--
ZE2 class type hinting non existing class
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class Foo {
	function a(NonExisting $foo) {}
}

$o = new Foo;
$o->a($o);
?>
--EXPECTF--

Fatal error: Class 'NonExisting' not found in %stype_hinting_002.php on line %d
