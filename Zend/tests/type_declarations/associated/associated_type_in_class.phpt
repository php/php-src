--TEST--
Associated types in class is invalid
--FILE--
<?php

class C {
    type T;
    public function foo(T $param): T;
}

?>
--EXPECTF--
Fatal error: Cannot use associated types outside of interfaces, used in C in %s on line %d
