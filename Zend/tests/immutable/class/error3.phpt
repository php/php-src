--TEST--
Mutation of properties from immutable classes is not possible after construction.
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
$a->x = 2;
?>
--EXPECTF--

Fatal error: Uncaught Error: Cannot mutate immutable object X\Y\Z\A after construction in %simmutable%sclass%serror3.php:14
Stack trace:
#0 {main}
  thrown in %simmutable%sclass%serror3.php on line 14
