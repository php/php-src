--TEST--
Test assign to typed property taken by reference
--FILE--
<?php
class A {
    public $foo = 1;
    public int $bar = 2;
}
class B {
    public A $a;
}
$f = function (&$n) {
    var_dump($n);
    $n = "ops";
};
$o = new B;
$o->a = new A;
$f($o->a->foo);
$f($o->a->bar);
?>
--EXPECTF--
int(1)
int(2)

Fatal error: Uncaught TypeError: Cannot assign string to reference held by property A::$bar of type int in %s:%d
Stack trace:
#0 %s(%d): {closure}(2)
#1 {main}
  thrown in %s on line %d
