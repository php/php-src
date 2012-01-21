--TEST--
Bug #60825 (Segfault when running symfony 2 tests)
--DESCRIPTION--
run this with valgrind
--FILE--
<?php
if (isset($loaded)) {
    $loaded = true;
	class test {
		public function __toString() {
			return __FILE__;
		}
	}
	$a = new test;
	require_once $a;
}
?>
--EXPECT--
