--TEST--
Closure 040: Rebinding closures, bad arguments
--FILE--
<?php

class A {
	private $x;
	private static $xs = 10;
	
	public function __construct($v) {
		$this->x = $v;
	}
	
	public function getIncrementor() {
		return function() { return ++$this->x; };
	}
	public function getStaticIncrementor() {
		return static function() { return ++static::$xs; };
	}
}

$a = new A(20);

$ca = $a->getIncrementor();
$cas = $a->getStaticIncrementor();

$ca->bindTo($a, array());
$ca->bindTo(array(), 'A');
$ca->bindTo($a, array(), "");
$ca->bindTo();
$cas->bindTo($a, 'A');

?>
--EXPECTF--
Notice: Array to string conversion in %s on line %d

Warning: Class 'Array' not found in %s on line %d

Warning: Closure::bindTo() expects parameter 1 to be object, array given in %s on line 25

Warning: Closure::bindTo() expects at most 2 parameters, 3 given in %s on line %d

Warning: Closure::bindTo() expects at least 1 parameter, 0 given in %s on line %d

Warning: Cannot bind an instance to a static closure in %s on line %d
