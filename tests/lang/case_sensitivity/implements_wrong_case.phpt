--TEST--
Implementing an interface with wrong-case name fails with wrong case
--FILE--
<?php
interface MyInterface {
    public function doSomething(): void;
}

class Child implements MYINTERFACE {
    public function doSomething(): void {}
}

echo "done\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Interface "MYINTERFACE" not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
