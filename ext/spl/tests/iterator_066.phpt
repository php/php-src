--TEST--
SPL: NoRewindIterator::__construct(void)
--CREDITS--
Sebastian Schürmann
--FILE--
<?php
class myNoRewindIterator extends NoRewindIterator  {}
try {
	$it = new myNoRewindIterator();
} catch (InvalidArgumentException $e) {
	echo 'InvalidArgumentException thrown';
}
?>
--EXPECT--
InvalidArgumentException thrown
