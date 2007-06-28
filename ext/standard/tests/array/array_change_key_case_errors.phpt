--TEST--
Test array_change_key_case() function - 3
--FILE--
<?php
/* generate different failure conditions */
$int_var  = -19;
$item = array ("one" => 1, "two" => 2, "THREE" => 3, "FOUR" => "four");

var_dump( array_change_key_case($int_var) ); // args less than expected
var_dump( array_change_key_case($int_var, CASE_UPPER) ); // invalid first argument
var_dump( array_change_key_case() ); // Zero argument
var_dump( array_change_key_case($item, $item["one"], "CASE_UPPER") ); // more than expected numbers

echo "end\n";
?>
--EXPECTF--
Warning: array_change_key_case(): The argument should be an array in %s on line %d
bool(false)

Warning: array_change_key_case(): The argument should be an array in %s on line %d
bool(false)

Warning: Wrong parameter count for array_change_key_case() in %s on line %d
NULL

Warning: Wrong parameter count for array_change_key_case() in %s on line %d
NULL
end
