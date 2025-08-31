--TEST--
A redeclared class constant must have the same or higher visibility
--FILE--
<?php

class A {
    public const publicConst = 0;
}

class B extends A {
    protected const publicConst = 1;
}
?>
--EXPECTF--
Fatal error: Access level to B::publicConst must be public (as in class A) in %s on line 7
