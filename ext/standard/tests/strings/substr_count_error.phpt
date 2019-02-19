--TEST--
Test substr_count() function (error conditions)
--FILE--
<?php

echo "\n*** Testing error conditions ***\n";
$str = 'abcdefghik';

/* offset before start */
var_dump(substr_count($str, "t", -20));

/* offset > size of the string */
var_dump(substr_count($str, "t", 25));

/* Using offset and length to go beyond the size of the string:
   Warning message expected, as length+offset > length of string */
var_dump( substr_count($str, "i", 5, 7) );

/* length too small */
var_dump( substr_count($str, "t", 2, -20) );

echo "Done\n";

?>
--EXPECTF--
*** Testing error conditions ***

Warning: substr_count(): Offset not contained in string in %s on line %d
bool(false)

Warning: substr_count(): Offset not contained in string in %s on line %d
bool(false)

Warning: substr_count(): Invalid length value in %s on line %d
bool(false)

Warning: substr_count(): Invalid length value in %s on line %d
bool(false)
Done
