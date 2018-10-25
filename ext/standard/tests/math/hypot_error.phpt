--TEST--
Test hypot() - wrong params test hypot()
--FILE--
<?php
/* Prototype  : float hypot  ( float $x  , float $y  )
 * Description: Calculate the length of the hypotenuse of a right-angle triangle.
 * Source code: ext/standard/math.c
 */

echo "*** Testing hypot() : error conditions ***\n";

echo "\n-- Testing hypot() function with less than expected no. of arguments --\n";
hypot();
hypot(36);

echo "\n-- Testing hypot() function with more than expected no. of arguments --\n";
hypot(36,25,0);

?>
===Done===
--EXPECTF--
*** Testing hypot() : error conditions ***

-- Testing hypot() function with less than expected no. of arguments --

Warning: hypot() expects exactly 2 parameters, 0 given in %s on line %d

Warning: hypot() expects exactly 2 parameters, 1 given in %s on line %d

-- Testing hypot() function with more than expected no. of arguments --

Warning: hypot() expects exactly 2 parameters, 3 given in %s on line %d
===Done===
