--TEST--
Referencing immutable property is not permitted.
--FILE--
<?php
namespace X\Y\Z;
immutable class A {
	public $x;
	public function __construct($x) {
		$this->x = $x;
	}
}
$a = new A(1);
$x = &$a->x;
?>
--EXPECTF--


Fatal error: Uncaught Error: Can not reference property of immutable object in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
