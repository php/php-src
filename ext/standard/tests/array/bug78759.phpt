--TEST--
Bug #78759: array_search in $GLOBALS
--FILE--
<?php

$a = 22;
var_dump($GLOBALS["a"]); // int 22
var_dump(array_search(22, $GLOBALS)); // false
var_dump(array_search(22, $GLOBALS, true)); // false

?>
--EXPECT--
int(22)
string(1) "a"
string(1) "a"
