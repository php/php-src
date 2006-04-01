--TEST--
SPL: ArrayIterator and values assigned by reference
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php
	
$tmp = 1;

$a = new ArrayIterator();
$a[] = $tmp;
$a[] = &$tmp;

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Objects used as arrays in post/pre increment/decrement must return values by reference in %s on line %d
