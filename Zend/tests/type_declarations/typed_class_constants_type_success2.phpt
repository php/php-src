--TEST--
Typed class constants (declaration; runtime)
--FILE--
<?php
class A {
    public const object CONST1 = C;
    public const ?object CONST2 = C;
    public const B|stdClass CONST3 = C;
    public const B&Stringable CONST4 = C;
    public const (B&Stringable)|null CONST5 = C;
}

class B implements Stringable {
    public function __toString() {
        return "";
    }
}

const C = new B();

var_dump(A::CONST1);
var_dump(A::CONST1);
var_dump(A::CONST2);
var_dump(A::CONST2);
var_dump(A::CONST3);
var_dump(A::CONST3);
var_dump(A::CONST4);
var_dump(A::CONST4);
var_dump(A::CONST5);
var_dump(A::CONST5);
?>
--EXPECTF--
object(B)#%d (%d) {
}
object(B)#%d (%d) {
}
object(B)#%d (%d) {
}
object(B)#%d (%d) {
}
object(B)#%d (%d) {
}
object(B)#%d (%d) {
}
object(B)#%d (%d) {
}
object(B)#%d (%d) {
}
object(B)#%d (%d) {
}
object(B)#%d (%d) {
}
