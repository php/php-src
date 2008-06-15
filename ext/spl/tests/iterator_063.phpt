--TEST--
SPL: LimitIterator::__construct(void)
--CREDITS--
Sebastian Schürmann
--FILE--
<?php
class myLimitIterator extends LimitIterator {
	
}
try {
	$it = new myLimitIterator();
} catch (InvalidArgumentException $e) {
	echo 'InvalidArgumentException thrown';
}
?>
--EXPECT--
InvalidArgumentException thrown
