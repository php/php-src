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
Catchable fatal error: Return value of qux::foo() must be an instance of foo, instance of qux returned in %s on line %d
