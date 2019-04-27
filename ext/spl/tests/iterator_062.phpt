--TEST--
SPL: RecursiveIteratorIterator::__construct(void)
--CREDITS--
Sebastian Schürmann
--FILE--
<?php
class myRecursiveIteratorIterator extends RecursiveIteratorIterator {

}

try {
	$it = new myRecursiveIteratorIterator();
} catch (InvalidArgumentException $e) {
	echo 'InvalidArgumentException thrown';
}
?>
--EXPECT--
InvalidArgumentException thrown
