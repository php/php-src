--TEST--
Test sizeof() function : basic functionality - for scalar types 
--FILE--
<?php
/* Prototype  : int sizeof(mixed $var[, int $mode] )
 * Description: Counts an elements in an array. If Standard PHP library is 
 *              installed, it will return the properties of an object.
 * Source code: ext/standard/basic_functions.c
 * Alias to functions: count()
 */

/* Testing the sizeof() for some of the scalar types(integer, float) values 
 * in default, COUNT_NORMAL and COUNT_RECURSIVE modes.
 */ 

echo "*** Testing sizeof() : basic functionality ***\n";

$intval = 10;
$floatval = 10.5;
$stringval = "String";

echo "-- Testing sizeof() for integer type in default, COUNT_NORMAL and COUNT_RECURSIVE modes --\n";
echo "default mode: ";
var_dump( sizeof($intval) );
echo "\n";
echo "COUNT_NORMAL mode: ";
var_dump( sizeof($intval, COUNT_NORMAL) );
echo "\n";
echo "COUNT_RECURSIVE mode: ";
var_dump( sizeof($intval, COUNT_RECURSIVE) );
echo "\n";

echo "-- Testing sizeof() for float  type in default, COUNT_NORMAL and COUNT_RECURSIVE modes --\n";
echo "default mode: ";
var_dump( sizeof($floatval) );
echo "\n";
echo "COUNT_NORMAL mode: ";
var_dump( sizeof($floatval, COUNT_NORMAL) );
echo "\n";
echo "COUNT_RECURSIVE mode: ";
var_dump( sizeof($floatval, COUNT_RECURSIVE) );

echo "Done";
?>
--EXPECTF--
*** Testing sizeof() : basic functionality ***
-- Testing sizeof() for integer type in default, COUNT_NORMAL and COUNT_RECURSIVE modes --
default mode: int(1)

COUNT_NORMAL mode: int(1)

COUNT_RECURSIVE mode: int(1)

-- Testing sizeof() for float  type in default, COUNT_NORMAL and COUNT_RECURSIVE modes --
default mode: int(1)

COUNT_NORMAL mode: int(1)

COUNT_RECURSIVE mode: int(1)
Done
