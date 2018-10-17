--TEST--
Return value fails inheritance check in method
--FILE--
<?php
class foo {}

class qux {
    public function foo() : foo {
        return $this;
    }
}

$qux = new qux();
$qux->foo();
--EXPECTF--
Fatal error: Uncaught TypeError: Return value of qux::foo() must be an instance of foo, instance of qux returned in %s:%d
Stack trace:
#0 %s(%d): qux->foo()
#1 {main}
  thrown in %s on line %d
