--TEST--
ZE2 interfaces
--FILE--
<?php

interface ThrowableInterface {
	public function getMessage();
}

class Exception_foo implements ThrowableInterface {
	public $foo = "foo";

	public function getMessage() {
		return $this->foo;
	}
}

$foo = new Exception_foo;
echo $foo->getMessage() . "\n";

?>
--EXPECT--
foo
