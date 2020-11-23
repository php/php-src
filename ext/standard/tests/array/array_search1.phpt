--TEST--
array_search() tests
--FILE--
<?php

$a = array(1=>0, 2=>1, 4=>3, "a"=>"b", "c"=>"d");

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
--EXPECT--
bool(false)
bool(false)
int(1)
int(1)
int(2)
string(1) "c"
string(1) "c"
bool(false)
Done
