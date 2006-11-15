--TEST--
array_key_exists() tests
--FILE--
<?php

var_dump(array_key_exists());
var_dump(array_key_exists(array(), array()));
var_dump(array_key_exists("", ""));
var_dump(array_key_exists("", array()));
var_dump(array_key_exists(1, array()));

var_dump(array_key_exists(1, array(1,2,3)));
var_dump(array_key_exists("a", array(3,2,1,"a"=>1)));
var_dump(array_key_exists("a", array(3,2,1)));
var_dump(array_key_exists(NULL, array(5,6,7,""=>"value", 3,2,1)));
var_dump(array_key_exists(NULL, array(5,6,7,3,2,1)));
var_dump(array_key_exists(false, array(5,6,7,""=>"value", 3,2,1)));

echo "Done\n";
?>
--EXPECTF--	
Warning: Wrong parameter count for array_key_exists() in %s on line %d
NULL

Warning: array_key_exists(): The first argument should be either a string or an integer in %s on line %d
bool(false)

Warning: array_key_exists(): The second argument should be either an array or an object in %s on line %d
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)

Warning: array_key_exists(): The first argument should be either a string or an integer in %s on line %d
bool(false)
Done
