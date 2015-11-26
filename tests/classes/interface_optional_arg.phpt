--TEST--
ZE2 An interface method allows additional default arguments
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

error_reporting(4095);

interface test {
	public function bar();
}

class foo implements test {

	public function bar($foo = NULL) {
		echo "foo\n";
	}
}

$foo = new foo;
$foo->bar();

?>
--EXPECT--
foo

