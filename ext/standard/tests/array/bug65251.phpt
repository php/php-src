--TEST--
Bug #65251: array_merge_recursive() recursion detection broken
--FILE--
<?php

array_merge_recursive($GLOBALS, $GLOBALS)

?>
--EXPECTF--
Warning: array_merge_recursive(): recursion detected in %s on line %d
