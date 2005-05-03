--TEST--
ZE2 interface with an unimplemented method
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

interface Throwable {
	public function getMessage();
	public function getErrno();
}

class Exception_foo implements Throwable {
	public $foo = "foo";

	public function getMessage() {
		return $this->foo;
	}
}

// this should die -- Exception class must be abstract...
$foo = new Exception_foo;
echo "Message: " . $foo->getMessage() . "\n";

?>
===DONE===
--EXPECTF--

Fatal error: Class Exception_foo contains 1 abstract method and must therefore be declared abstract or implement the remaining methods (Throwable::getErrno) in %s on line %d
