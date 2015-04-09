--TEST--
SPL: RecursiveCachingIterator::__construct(void)
--CREDITS--
Sebastian Schürmann
--FILE--
<?php
class myRecursiveCachingIterator  extends RecursiveCachingIterator  {}
try {
	$it = new myRecursiveCachingIterator();
} catch (InvalidArgumentException $e) {
	echo 'InvalidArgumentException thrown';
}
?>
--EXPECT--
InvalidArgumentException thrown
