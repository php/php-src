--TEST--
Return value of assigning by-val function result by-reference
--FILE--
<?php
$a = [&$a];
var_dump($a[0] =& returnsVal());
function returnsVal() {}
?>
--EXPECTF--
Notice: Only variables should be assigned by reference in %s on line %d
NULL
