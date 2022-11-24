--TEST--
GC 047: Reference-less cyclic array
--INI--
zend.enable_gc=1
--XFAIL--
Creates a reference-less cyclic array, not sure how to avoid without touching hot paths
--FILE--
<?php
$a = null;
$b = &$a;
$a[] = $b;
?>
DONE
--EXPECT--
DONE
