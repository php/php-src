--TEST--
Cannot replace readwrite with readonly
--FILE--
<?php

class A {
    public int $prop;
}
class B extends A {
    public readonly int $prop;
}

?>
--EXPECTF--
Fatal error: Cannot redeclare non-readonly property A::$prop as readonly B::$prop in %s on line %d
