--TEST--
public(set) is forbidden on non-readonly
--FILE--
<?php

class A {
    public public(set) int $prop;
}

?>
--EXPECTF--
Fatal error: Non-readonly property A::$prop may not use the public(set) modifier in %s on line %d
