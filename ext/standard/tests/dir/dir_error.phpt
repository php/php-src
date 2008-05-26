--TEST--
Test dir() function : error conditions 
--FILE--
<?php
/* 
 * Prototype  : object dir(string $directory[, resource $context])
 * Description: Directory class with properties, handle and class and methods read, rewind and close
 * Source code: ext/standard/dir.c
 */

echo "*** Testing dir() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing dir() function with zero arguments --";
var_dump( dir() );

// With one more than expected number of arguments
echo "\n-- Testing dir() function with one more than expected number of arguments --";
$extra_arg = 10;
var_dump( dir(getcwd(), "stream", $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing dir() : error conditions ***

-- Testing dir() function with zero arguments --
Warning: dir() expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing dir() function with one more than expected number of arguments --
Warning: dir() expects at most 2 parameters, 3 given in %s on line %d
NULL
Done
