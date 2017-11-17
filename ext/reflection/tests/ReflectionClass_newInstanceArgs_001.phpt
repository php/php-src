--TEST--
ReflectionClass::newInstanceArgs
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class A {
	public function A() {
		echo "In constructor of class A\n"; 
	}	
}

class B {
	public function __construct($a, $b) {
		echo "In constructor of class B with args $a, $b\n";
	}
}

class C {
	protected function __construct() {
		echo "In constructor of class C\n"; 
	}
}

class D {
	private function __construct() {
		echo "In constructor of class D\n";		
	}
}
class E {	
}


$rcA = new ReflectionClass('A');
$rcB = new ReflectionClass('B');
$rcC = new ReflectionClass('C');
$rcD = new ReflectionClass('D');
$rcE = new ReflectionClass('E');

try {
	var_dump($rcA->newInstanceArgs());
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}
try {
	var_dump($rcA->newInstanceArgs(array('x')));
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}

try {
	var_dump($rcB->newInstanceArgs());
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}
try {
	var_dump($rcB->newInstanceArgs(array('x', 123)));
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}

try {
	$rcC->newInstanceArgs();
	echo "you should not see this\n";
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}

try {
	$rcD->newInstanceArgs();
	echo "you should not see this\n";
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}

$e1 = $rcE->newInstanceArgs();
var_dump($e1);

try {
	$e2 = $rcE->newInstanceArgs(array('x'));
	echo "you should not see this\n";
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
--EXPECTF--
Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; A has a deprecated constructor in %s on line %d
In constructor of class A
object(A)#%d (0) {
}
In constructor of class A
object(A)#%d (0) {
}
Exception: Too few arguments to function B::__construct(), 0 passed and exactly 2 expected
In constructor of class B with args x, 123
object(B)#%d (0) {
}
Access to non-public constructor of class C
Access to non-public constructor of class D
object(E)#%d (0) {
}
Class E does not have a constructor, so you cannot pass any constructor arguments
