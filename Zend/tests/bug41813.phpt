--TEST--
Bug #41813 (segmentation fault when using string offset as an object)
--FILE--
<?php

$foo = "50";
$foo[0]->bar = "xyz";

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Cannot use string offset as an array in %s on line %d
