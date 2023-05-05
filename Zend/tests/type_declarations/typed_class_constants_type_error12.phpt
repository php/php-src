--TEST--
Typed class constants with static in union
--FILE--
<?php
class A {
    public const static|B CONST1 = B;
    public const static|stdClass|B CONST2 = B;
}
class B {}
define("B", new B());
var_dump(new A());
?>
--EXPECT--
object(A)#2 (0) {
}
