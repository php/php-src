--TEST--
ZE2 interfaces
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

interface Throwable {
	public function getMessage();
}

class Exception implements Throwable {
	public $foo = "foo";

	public function getMessage() {
		return $this->foo;
	}
}

$foo = new Exception;
echo $foo->getMessage() . "\n";

?>
--EXPECT--
foo

