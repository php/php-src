--TEST--
SPL: FilterIterator::__construct(void)
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--CREDITS--
Sebastian Schürmann
--FILE--
<?php
class myFilterIterator extends FilterIterator {
	function accept() {
		
	}
}
try {
	$it = new myFilterIterator();	
} catch (InvalidArgumentException $e) {
	echo 'InvalidArgumentException thrown';
}
?>
--EXPECT--
InvalidArgumentException thrown
