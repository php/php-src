--TEST--
Typed class constants (inheritance success - object types)
--FILE--
<?php
class S implements Stringable {
    public function __toString() {
        return "";
    }
}

class Z extends S {}

class A {
    public const object CONST1 = S;
    public const S CONST2 = S;
    public const S|Stringable CONST3 = S;
    public const S CONST4 = S;
    public const ?S CONST5 = S;
}

class B extends A {
    public const S CONST1 = Z;
    public const Z CONST2 = Z;
    public const S CONST3 = Z;
    public const S&Stringable CONST4 = Z;
    public const (S&Stringable)|null CONST5 = Z;
}

define("S", new S());
define("Z", new Z());

var_dump(B::CONST1);
var_dump(B::CONST2);
var_dump(B::CONST3);
var_dump(B::CONST4);
var_dump(B::CONST5);
?>
--EXPECTF--
object(Z)#%d (%d) {
}
object(Z)#%d (%d) {
}
object(Z)#%d (%d) {
}
object(Z)#%d (%d) {
}
object(Z)#%d (%d) {
}