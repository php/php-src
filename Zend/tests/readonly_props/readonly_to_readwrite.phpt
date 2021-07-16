--TEST--
Cannot replace readonly with readwrite
--FILE--
<?php

class A {
    public readonly int $prop;
}
class B extends A {
    public int $prop;
}

?>
--EXPECTF--
Fatal error: Cannot redeclare readonly property A::$prop as non-readonly B::$prop in %s on line %d
