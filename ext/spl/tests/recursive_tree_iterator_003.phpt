--TEST--
SPL: RecursiveTreeIterator(non-traversable)
--INI--
error_reporting=E_ALL&~E_NOTICE
--FILE--
<?php
try {
	new RecursiveTreeIterator(new ArrayIterator(array()));
} catch (InvalidArgumentException $e) {
	echo "InvalidArgumentException thrown\n";
}
?>
===DONE===
--EXPECTF--
InvalidArgumentException thrown
===DONE===
