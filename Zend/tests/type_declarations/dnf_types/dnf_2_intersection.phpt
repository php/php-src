--TEST--
Union of two intersection type
--FILE--
<?php

interface W {}
interface X {}
interface Y {}
interface Z {}

class A implements X, Y {}
class B implements W, Z {}
class C {}

function foo1((X&Y)|(W&Z) $v): (X&Y)|(W&Z) {
    return $v;
}
function foo2((W&Z)|(X&Y) $v): (W&Z)|(X&Y) {
    return $v;
}

function bar1(): (X&Y)|(W&Z) {
    return new C();
}
function bar2(): (W&Z)|(X&Y) {
    return new C();
}

$a = new A();
$b = new B();

$o = foo1($a);
var_dump($o);
$o = foo2($a);
var_dump($o);
$o = foo1($b);
var_dump($o);
$o = foo2($b);
var_dump($o);

try {
    bar1();
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    bar2();
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

?>
--EXPECTF--
object(A)#%d (0) {
}
object(A)#%d (0) {
}
object(B)#%d (0) {
}
object(B)#%d (0) {
}
bar1(): Return value must be of type (X&Y)|(W&Z), C returned
bar2(): Return value must be of type (W&Z)|(X&Y), C returned
