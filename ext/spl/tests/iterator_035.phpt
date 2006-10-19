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
Fatal error: Cannot assign by reference to overloaded object in %s on line %d
