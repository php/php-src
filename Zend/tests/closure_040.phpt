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
$cas->bindTo($a, 'A');

?>
--EXPECTF--
Warning: Array to string conversion in %s on line %d

Warning: Class 'Array' not found in %s on line %d

Warning: Cannot bind an instance to a static closure in %s on line %d
