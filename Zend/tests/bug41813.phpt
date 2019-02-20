--TEST--
Bug #41813 (segmentation fault when using string offset as an object)
--FILE--
<?php

$foo = "50";
$foo[0]->bar = "xyz";

echo "Done\n";
?>
--EXPECTF--
Warning: Attempt to assign property 'bar' of non-object in %s on line %d
Done
