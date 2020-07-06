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
--EXPECTF--
Fatal error: Constant B::publicConst must have public visibility to be compatible with overridden constant A::publicConst in %s on line %d
