--TEST--
ZE2 interface with an unimplemented method
--FILE--
<?php

interface ThrowableInterface {
    public function getMessage();
    public function getErrno();
}

class Exception_foo implements ThrowableInterface {
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
Fatal error: Class Exception_foo contains 1 abstract method and must therefore be declared abstract or implement the remaining methods (ThrowableInterface::getErrno) in %s on line %d
