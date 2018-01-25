--TEST--
array_search() tests
--FILE--
<?php

$a = array(1=>0, 2=>1, 4=>3, "a"=>"b", "c"=>"d");

var_dump(array_search(1));
var_dump(array_search(1,1));
var_dump(array_search("a",$a));
var_dump(array_search("0",$a, true));
var_dump(array_search("0",$a));
var_dump(array_search(0,$a));
var_dump(array_search(1,$a));
var_dump(array_search("d",$a, true));
var_dump(array_search("d",$a));
var_dump(array_search(-1,$a, true));

echo "Done\n";
?>
--EXPECTF--
Warning: array_search() expects at least 2 parameters, 1 given in %s on line %d
NULL

Warning: array_search() expects parameter 2 to be array, integer given in %s on line %d
NULL
int(1)
bool(false)
int(1)
int(1)
int(2)
string(1) "c"
int(1)
bool(false)
Done
