--TEST--
Closure 037: self:: and static:: within closures
--FILE--
<?php
class A {
	private $x = 0;

	function getClosure () {
			return function () {
				$this->x++;
				self::printX();
				self::print42();
				static::print42();
			};
	}

	function printX () {
		echo $this->x."\n";
	}

	function print42() {
		echo "42\n";
	}
}

class B extends A {
	function print42() {
		echo "forty two\n";
	}
}

$a = new A;
$closure = $a->getClosure();
$closure();
$b = new B;
$closure = $b->getClosure();
$closure();
?>
Done.
--EXPECT--
1
42
42
1
42
forty two
Done.
