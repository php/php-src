--TEST--
errmsg: cannot reassign $this (by ref)
--FILE--
<?php

class test {
	function foo() {
		$a = new test;
		$this = &$a;
	}
}

$t = new test;
$t->foo();

echo "Done\n";
?>
--EXPECTF--
Fatal error: Cannot re-assign $this in %s on line %d
