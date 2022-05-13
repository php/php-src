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
?>
--EXPECTF--
Fatal error: Access level to B::protectedConst must be protected (as in class A) or weaker in %s on line 7
