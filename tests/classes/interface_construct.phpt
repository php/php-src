--TEST--
ZE2 An interface constructor signature must not be inherited
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php
error_reporting(4095);

interface test {
	public function __construct($foo);
}

class foo implements test {
	public function __construct() {
		echo "foo\n";
	}
}

$foo = new foo;

?>
--EXPECT--
foo

