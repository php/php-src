--TEST--
Test substr_count() function (error conditions)
--FILE--
<?php

echo "\n*** Testing error conditions ***\n";
/* Zero argument */
var_dump( substr_count() );

/* more than expected no. of args */
var_dump( substr_count($str, "t", 0, 15, 30) );
	
/* offset as negative value */
var_dump(substr_count($str, "t", -5));

/* offset > size of the string */
var_dump(substr_count($str, "t", 25));

/* Using offset and length to go beyond the size of the string: 
   Warning message expected, as length+offset > length of string */
var_dump( substr_count($str, "i", 5, 15) );

/* length as Null */
var_dump( substr_count($str, "t", "", "") );
var_dump( substr_count($str, "i", NULL, NULL) );
	
echo "Done\n";	

?>
--EXPECTF--
*** Testing error conditions ***

Warning: Wrong parameter count for substr_count() in %s on line %d
NULL

Notice: Undefined variable: str in %s on line %d

Warning: Wrong parameter count for substr_count() in %s on line %d
NULL

Notice: Undefined variable: str in %s on line %d

Warning: substr_count(): Offset should be greater than or equal to 0 in %s on line %d
bool(false)

Notice: Undefined variable: str in %s on line %d

Warning: substr_count(): Offset value 25 exceeds string length in %s on line %d
bool(false)

Notice: Undefined variable: str in %s on line %d

Warning: substr_count(): Offset value 5 exceeds string length in %s on line %d
bool(false)

Notice: Undefined variable: str in %s on line %d

Warning: substr_count(): Length should be greater than 0 in %s on line %d
bool(false)

Notice: Undefined variable: str in %s on line %d

Warning: substr_count(): Length should be greater than 0 in %s on line %d
bool(false)
Done
