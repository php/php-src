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
?>
--EXPECTF--
Fatal error: Uncaught TypeError: qux::foo(): Return value must be of type foo, qux returned in %s:%d
Stack trace:
#0 %s(%d): qux->foo()
#1 {main}
  thrown in %s on line %d
