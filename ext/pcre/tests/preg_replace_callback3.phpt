--TEST--
preg_replace_callback() 3
--FILE--
<?php

var_dump(preg_replace_callback());
var_dump(preg_replace_callback(1));
var_dump(preg_replace_callback(1,2));
var_dump(preg_replace_callback(1,2,3));
var_dump(preg_replace_callback(1,2,3,4));
$a = 5;
var_dump(preg_replace_callback(1,2,3,4,$a));
$a = "";
var_dump(preg_replace_callback("","","","",$a));
$a = array();
var_dump(preg_replace_callback($a,$a,$a,$a,$a));

echo "Done\n";
?>
--EXPECTF--
Warning: preg_replace_callback() expects at least 3 parameters, 0 given in %s on line %d
NULL

Warning: preg_replace_callback() expects at least 3 parameters, 1 given in %s on line %d
NULL

Warning: preg_replace_callback() expects at least 3 parameters, 2 given in %s on line %d
NULL

Warning: preg_replace_callback(): Requires argument 2, '2', to be a valid callback in %s on line %d
string(1) "3"

Warning: preg_replace_callback(): Requires argument 2, '2', to be a valid callback in %s on line %d
string(1) "3"

Warning: preg_replace_callback(): Requires argument 2, '2', to be a valid callback in %s on line %d
string(1) "3"

Warning: preg_replace_callback() expects parameter 4 to be integer, string given in %s on line %d
NULL

Warning: preg_replace_callback() expects parameter 4 to be integer, array given in %s on line %d
NULL
Done
