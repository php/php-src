--TEST--
Test array_last_key() function
--FILE--
<?php
$a = [];
var_dump( array_last_key($a) );

$a[] = 7; /* it shouldn't matter what the value is, so I'm using 7 */
var_dump( array_last_key($a) );

$a[1] = 7;
var_dump( array_last_key($a) );

$a = ["foobar" => 7];
var_dump( array_last_key($a) );

echo "Done\n";
?>
--EXPECTF--
NULL
int(0)
int(1)
string(6) "foobar"
Done
