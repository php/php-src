--TEST--
Associated types in trait is invalid
--FILE--
<?php

trait C {
    type T;
    public function foo(T $param): T;
}

?>
--EXPECTF--
Fatal error: Cannot use associated types outside of interfaces, used in C in %s on line %d
