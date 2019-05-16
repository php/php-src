--TEST--
SPL: SplHeap top, corrupted heap
--CREDITS--
Mark Schaschke (mark@fractalturtle.com)
TestFest London May 2009
--FILE--
<?php
// override heap to force corruption by throwing exception in compare
class SplMinHeap2 extends SplMinHeap {
	public function compare($a, $b) {
		throw new Exception('Corrupt heap');
	}
}

$h = new SplMinHeap2();

// insert 2 elements to hit our overridden compare
$h->insert(4);
try {
	$h->insert(5);
} catch (Exception $e) {}

// call top, should fail with corrupted heap
try {
	$h->top();
} catch (Exception $e) {
	echo $e->getMessage();
}
?>
--EXPECT--
Heap is corrupted, heap properties are no longer ensured.
