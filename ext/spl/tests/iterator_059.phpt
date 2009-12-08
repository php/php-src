--TEST--
SPL: CachingIterator::__construct(void)
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--CREDITS--
Sebastian Schürmann
--FILE--
<?php
class myCachingIterator extends CachingIterator {
	
}
try {
	$it = new myCachingIterator();	
} catch (InvalidArgumentException $e) {
	echo 'InvalidArgumentException thrown';
}
?>
--EXPECT--
InvalidArgumentException thrown
