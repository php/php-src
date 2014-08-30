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
Catchable fatal error: The function qux::foo was expected to return an object of class foo and returned an object of class qux in %s on line %d
