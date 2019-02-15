--TEST--
SPL: SplMinHeap compare, illegal number of args
--CREDITS--
Mark Schaschke (mark@fractalturtle.com)
TestFest London May 2009
--FILE--
<?php
class SplMinHeap2 extends SplMinHeap {
	public function testCompare1() {
		return parent::compare();
	}
	public function testCompare2() {
		return parent::compare(1);
	}
	public function testCompare3() {
		return parent::compare(1, 2, 3);
	}
}

$h = new SplMinHeap2();
$h->testCompare1();
$h->testCompare2();
$h->testCompare3();
?>
--EXPECTF--
Warning: SplMinHeap::compare() expects exactly 2 parameters, 0 given in %s

Warning: SplMinHeap::compare() expects exactly 2 parameters, 1 given in %s

Warning: SplMinHeap::compare() expects exactly 2 parameters, 3 given in %s
