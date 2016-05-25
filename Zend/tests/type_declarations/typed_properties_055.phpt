--TEST--
Default values of callable properties
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
	$i = "ops";
};
$o = new B;
$o->a = new A;
$f($o->a->foo);
$f($o->a->bar);
?>
--EXPECTF--
int(1)

Fatal error: Uncaught TypeError: Typed property A::$bar must not be referenced in %styped_properties_055.php:16
Stack trace:
#0 {main}
  thrown in %styped_properties_055.php on line 16
