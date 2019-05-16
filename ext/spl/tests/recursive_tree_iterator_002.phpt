--TEST--
SPL: RecursiveTreeIterator(void)
--INI--
error_reporting=E_ALL&~E_NOTICE
--FILE--
<?php
try {
	new RecursiveTreeIterator();
} catch (InvalidArgumentException $e) {
	echo "InvalidArgumentException thrown\n";
}
?>
===DONE===
--EXPECT--
InvalidArgumentException thrown
===DONE===
