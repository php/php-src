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
echo $foo->getMessage() . "\n";

?>
--EXPECTF--

Fatal error: Class exception_foo contains 1 abstract methods and must therefore be declared abstract (throwable::geterrno) in %s on line %d

