--TEST--
Test substr_count() function (error conditions)
--FILE--
<?php

echo "\n*** Testing error conditions ***\n";
$str = 'abcdefghik';

/* Zero argument */
var_dump( substr_count() );

/* more than expected no. of args */
var_dump( substr_count($str, "t", 0, 15, 30) );
	
/* offset before start */
var_dump(substr_count($str, "t", -20));

/* offset > size of the string */
var_dump(substr_count($str, "t", 25));

/* Using offset and length to go beyond the size of the string: 
   Warning message expected, as length+offset > length of string */
var_dump( substr_count($str, "i", 5, 7) );

/* Invalid offset argument */
var_dump( substr_count($str, "t", "") );

/* length too small */
var_dump( substr_count($str, "t", 2, -20) );

echo "Done\n";

?>
--EXPECTF--
*** Testing error conditions ***

Warning: substr_count() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: substr_count() expects at most 4 parameters, 5 given in %s on line %d
NULL

Warning: substr_count(): Offset not contained in string in %s on line %d
bool(false)

Warning: substr_count(): Offset not contained in string in %s on line %d
bool(false)

Warning: substr_count(): Invalid length value in %s on line %d
bool(false)

Warning: substr_count() expects parameter 3 to be integer, string given in %s on line %d
NULL

Warning: substr_count(): Invalid length value in %s on line %d
bool(false)
Done
