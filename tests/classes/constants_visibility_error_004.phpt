--TEST--
A redeclared class constant must have the same or higher visibility
--FILE--
<?php

class A {
    protected const protectedConst = 0;
}

class B extends A {
    private const protectedConst = 1;
}
--EXPECTF--
Fatal error: Constant B::protectedConst must have protected or public visibility to be compatible with overridden constant A::protectedConst in %s on line %d
