--TEST--
Test array_first_key() function
--FILE--
<?php
$a = [];
var_dump( array_first_key($a) );

$a[] = 7; /* it shouldn't matter what the value is, so I'm using 7 */
var_dump( array_first_key($a) );

$a = ["foobar" => 7];
var_dump( array_first_key($a) );

$a = [1, 2, 3, 4, 5, 6];
var_dump( array_first_key($a) );

echo "Done\n";
?>
--EXPECTF--
NULL
int(0)
string(6) "foobar"
int(0)
Done
