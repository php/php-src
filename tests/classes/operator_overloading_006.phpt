--TEST--
Userspace operator overloading - Type test
--FILE--
<?php

class A
{
    public static function __concat($lhs, $rhs)
    {
        var_dump($lhs);
        var_dump($rhs);
        return true;
    }
}

class B {}

$a = new A();
$b = new B();
$a . 1;
1 . $a;
$a . 1.0;
1.0 . $a;
$a . $a;
$a . $b;
$a . [];
[] . $a;

--EXPECT--
object(A)#1 (0) {
}
int(1)
int(1)
object(A)#1 (0) {
}
object(A)#1 (0) {
}
float(1)
float(1)
object(A)#1 (0) {
}
object(A)#1 (0) {
}
object(A)#1 (0) {
}
object(A)#1 (0) {
}
object(B)#2 (0) {
}
object(A)#1 (0) {
}
array(0) {
}
array(0) {
}
object(A)#1 (0) {
}
