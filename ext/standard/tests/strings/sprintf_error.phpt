--TEST--
Test sprintf() function : error conditions
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string
 * Source code: ext/standard/formatted_print.c
 */

echo "*** Testing sprintf() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing sprintf() function with Zero arguments --\n";
var_dump( sprintf() );

echo "\n-- Testing sprintf() function with less than expected no. of arguments --\n";
$format1 = '%s';
$format2 = '%s%s';
$format3 = '%s%s%s';
$arg1 = 'one';
$arg2 = 'two';

// with one argument less than expected
var_dump( sprintf($format1) );
var_dump( sprintf($format2,$arg1) );
var_dump( sprintf($format3,$arg1,$arg2) );

// with two argument less than expected
var_dump( sprintf($format2) );
var_dump( sprintf($format3,$arg1) );

// with three argument less than expected
var_dump( sprintf($format3) );

echo "Done";
?>
--EXPECTF--
*** Testing sprintf() : error conditions ***

-- Testing sprintf() function with Zero arguments --

Warning: sprintf() expects at least %d parameter, %d given in %s on line %d
bool(false)

-- Testing sprintf() function with less than expected no. of arguments --

Warning: sprintf(): Too few arguments in %s on line %d
bool(false)

Warning: sprintf(): Too few arguments in %s on line %d
bool(false)

Warning: sprintf(): Too few arguments in %s on line %d
bool(false)

Warning: sprintf(): Too few arguments in %s on line %d
bool(false)

Warning: sprintf(): Too few arguments in %s on line %d
bool(false)

Warning: sprintf(): Too few arguments in %s on line %d
bool(false)
Done
