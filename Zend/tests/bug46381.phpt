--TEST--
Bug #46381 (wrong $this passed to internal methods causes segfault)
--SKIPIF--
<?php if (!extension_loaded("spl")) die("skip SPL is no available"); ?>
--FILE--
<?php

class test {
	public function test() {
		return ArrayIterator::current();
	}
}
$test = new test();
$test->test();

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Non-static method ArrayIterator::current() cannot be called statically, assuming $this from incompatible context in %s on line %d
