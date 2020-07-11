--TEST--
Bug #76383: array_map on $GLOBALS returns IS_INDIRECT
--FILE--
<?php

$a = 1;
array_map(function($x) use (&$lastval) { $lastval = $x; }, $GLOBALS);
var_dump(gettype($lastval), $lastval); // will contain $a

?>
--EXPECT--
string(7) "integer"
int(1)
