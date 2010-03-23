--TEST--
Bug #45139 (ReflectionProperty returns incorrect declaring class)
--FILE--
<?php

class A {
	private $foo;
}

class B extends A {
	protected $bar;
	private $baz;
	private $quux;
}

class C extends B {
	public $foo;
	private $baz;
	protected $quux;
}

$rc = new ReflectionClass('C');
$rp = $rc->getProperty('foo');
var_dump($rp->getDeclaringClass()->getName()); // c

$rc = new ReflectionClass('A');
$rp = $rc->getProperty('foo');
var_dump($rp->getDeclaringClass()->getName()); // A

$rc = new ReflectionClass('B');
$rp = $rc->getProperty('bar');
var_dump($rp->getDeclaringClass()->getName()); // B

$rc = new ReflectionClass('C');
$rp = $rc->getProperty('bar');
var_dump($rp->getDeclaringClass()->getName()); // B

$rc = new ReflectionClass('C');
$rp = $rc->getProperty('baz');
var_dump($rp->getDeclaringClass()->getName()); // C

$rc = new ReflectionClass('B');
$rp = $rc->getProperty('baz');
var_dump($rp->getDeclaringClass()->getName()); // B

$rc = new ReflectionClass('C');
$rp = $rc->getProperty('quux');
var_dump($rp->getDeclaringClass()->getName()); // C

?>
--EXPECT--
string(1) "C"
string(1) "A"
string(1) "B"
string(1) "B"
string(1) "C"
string(1) "B"
string(1) "C"
