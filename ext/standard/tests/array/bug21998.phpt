--TEST--
Bug #21998 (array_pop() does not reset the current array position)
--FILE--
<?php

$a = array("a", "b", "c");

var_dump(key($a));
var_dump(array_pop($a));
var_dump(key($a));      
var_dump(array_pop($a));
var_dump(key($a));      
var_dump(array_pop($a));
var_dump(key($a));      

?>
--EXPECT--
int(0)
string(1) "c"
int(0)
string(1) "b"
int(0)
string(1) "a"
NULL
