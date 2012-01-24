--TEST--
Bug #60825 (Segfault when running symfony 2 tests)
--DESCRIPTION--
run this with valgrind
--FILE--
<?php
class test {
	public static $x;
	public function __toString() {
		self::$x = $this;
		return __FILE__;
	}
}
$a = new test;
require_once $a;
debug_zval_dump(test::$x);
?>
--EXPECTF--
string(%d) "%sbug60825.php" refcount(2)
