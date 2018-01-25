--TEST--
Bug #46381 (wrong $this passed to internal methods causes segfault)
--FILE--
<?php

class test {
	public function method() {
		return ArrayIterator::current();
	}
}
$test = new test();
$test->method();

echo "Done\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Non-static method ArrayIterator::current() cannot be called statically in %s:%d
Stack trace:
#0 %s(%d): test->method()
#1 {main}
  thrown in %s on line %d
