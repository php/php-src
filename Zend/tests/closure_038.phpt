--TEST--
Closure 038: Rebinding closures, change scope, different runtime type
--FILE--
<?php

class A {
	private $x;

	public function __construct($v) {
		$this->x = $v;
	}

	public function getIncrementor() {
		return function() { return ++$this->x; };
	}
}
class B extends A {
	private $x;
	public function __construct($v) {
		parent::__construct($v);
		$this->x = $v*2;
	}
}

$a = new A(0);
$b = new B(10);

$ca = $a->getIncrementor();
var_dump($ca());

echo "Testing with scope given as object", "\n";

$cb = $ca->bindTo($b, $b);
$cb2 = Closure::bind($ca, $b, $b);
var_dump($cb());
var_dump($cb2());

echo "Testing with scope as string", "\n";

$cb = $ca->bindTo($b, 'B');
$cb2 = Closure::bind($ca, $b, 'B');
var_dump($cb());
var_dump($cb2());

$cb = $ca->bindTo($b, NULL);
var_dump($cb());

?>
--EXPECTF--
int(1)
Testing with scope given as object
int(21)
int(22)
Testing with scope as string
int(23)
int(24)

Fatal error: Uncaught Error: Cannot access private property B::$x in %s:%d
Stack trace:
#0 %s(%d): Closure->{closure}()
#1 {main}

Next Error: Cannot access private property B::$x in %s:%d
Stack trace:
#0 %s(%d): Closure->{closure}()
#1 {main}
  thrown in %s on line %d
