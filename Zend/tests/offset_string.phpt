--TEST--
using different variables to access string offsets
--FILE--
<?php

$str = "Sitting on a corner all alone, staring from the bottom of his soul";

var_dump($str[1]);
var_dump($str[0.0836]);
var_dump($str[NULL]);
var_dump($str["run away"]);
var_dump($str["13"]);
var_dump($str["14.5"]);
var_dump($str["15 and then some"]);

var_dump($str[TRUE]);
var_dump($str[FALSE]);

$fp = fopen(__FILE__, "r");
var_dump($str[$fp]);

$obj = new stdClass;
var_dump($str[$obj]);

$arr = Array(1,2,3);
var_dump($str[$arr]);

echo "Done\n";
?>
--EXPECTF--
string(1) "i"

Notice: String offset cast occurred in %s on line %d
string(1) "S"

Notice: String offset cast occurred in %s on line %d
string(1) "S"

Warning: Illegal string offset 'run away' in %s on line %d
string(1) "S"
string(1) "c"

Warning: Illegal string offset '14.5' in %s on line %d
string(1) "o"

Notice: A non well formed numeric value encountered in %s on line %d
string(1) "r"

Notice: String offset cast occurred in %s on line %d
string(1) "i"

Notice: String offset cast occurred in %s on line %d
string(1) "S"

Warning: Illegal offset type in %s on line %d
string(1) "%s"

Warning: Illegal offset type in %s on line %d

Notice: Object of class stdClass could not be converted to int in %s on line %d
string(1) "%s"

Warning: Illegal offset type in %s on line %d
string(1) "i"
Done