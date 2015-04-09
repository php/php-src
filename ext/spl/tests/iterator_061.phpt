--TEST--
SPL: ParentIterator::__construct(void)
--CREDITS--
Sebastian Schürmann
--FILE--
<?php
class myParentIterator extends ParentIterator {
	
}
try {
	$it = new myParentIterator();	
} catch (InvalidArgumentException $e) {
	echo 'InvalidArgumentException thrown';
}
?>
--EXPECT--
InvalidArgumentException thrown
