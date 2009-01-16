--TEST--
Test printf() function : error conditions 
--FILE--
<?php
/* Prototype  : int printf  ( string $format  [, mixed $args  [, mixed $...  ]] )
 * Description: Produces output according to format .
 * Source code: ext/standard/formatted_print.c
 */

echo "*** Testing printf() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing printf() function with Zero arguments --\n";
var_dump( printf() );

echo "\n-- Testing printf() function with less than expected no. of arguments --\n";
$format1 = '%s';
$format2 = '%s%s';
$format3 = '%s%s%s';
$arg1 = 'one';
$arg2 = 'two';

echo "\n-- Call printf with one argument less than expected --\n";
var_dump( printf($format1) );  
var_dump( printf($format2,$arg1) );
var_dump( printf($format3,$arg1,$arg2) );

echo "\n-- Call printf with two argument less than expected --\n";
var_dump( printf($format2) );
var_dump( printf($format3,$arg1) );

echo "\n-- Call printf with three argument less than expected --\n";
var_dump( printf($format3) );

?>
===DONE===
--EXPECTF--
*** Testing printf() : error conditions ***

-- Testing printf() function with Zero arguments --

Warning: printf() expects at least 1 parameter, 0 given in %s on line %d
bool(false)

-- Testing printf() function with less than expected no. of arguments --

-- Call printf with one argument less than expected --

Warning: printf(): Too few arguments in %s on line %d
bool(false)

Warning: printf(): Too few arguments in %s on line %d
bool(false)

Warning: printf(): Too few arguments in %s on line %d
bool(false)

-- Call printf with two argument less than expected --

Warning: printf(): Too few arguments in %s on line %d
bool(false)

Warning: printf(): Too few arguments in %s on line %d
bool(false)

-- Call printf with three argument less than expected --

Warning: printf(): Too few arguments in %s on line %d
bool(false)
===DONE===