--TEST--
Bug #39067 (getDeclaringClass() and private properties)
--FILE--
<?php

class A {
	private $x;
}

class B extends A {
	private $x;
}

class C extends B {
	private $x;
}

$rc = new ReflectionClass('C');
var_dump($rc->getProperty('x')->getDeclaringClass()->getName());

$rc = new ReflectionClass('B');
var_dump($rc->getProperty('x')->getDeclaringClass()->getName());

$rc = new ReflectionClass('A');
var_dump($rc->getProperty('x')->getDeclaringClass()->getName());

class Test {
	private $x;
}

class Test2 extends Test {
	public $x;
}

$rc = new ReflectionClass('Test2');
var_dump($rc->getProperty('x')->getDeclaringClass()->getName());

echo "Done\n";
?>
--EXPECT--
string(1) "C"
string(1) "B"
string(1) "A"
string(5) "Test2"
Done
