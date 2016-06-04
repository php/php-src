--TEST--
Bug #70967 (Weird error handling for __toString when Error is thrown)
--FILE--
<?php
class A {
	public function __toString() {
		undefined_function();
	}
}

echo (new A);
?>
--EXPECTF--
Fatal error: Method A::__toString() must not throw an exception, caught Error: Call to undefined function undefined_function() in %sbug70967.php on line %d
