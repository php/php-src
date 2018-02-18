--TEST--
Reference assignment of immutable class properties are not permitted.
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

Fatal error: Uncaught Error: Reference assignment of immutable property X\Y\Z\A::$x is not permitted in %simmutable_modifier%sclass%serror6.php:15
Stack trace:
#0 {main}
  thrown in %simmutable%sclass%serror6.php on line 15
