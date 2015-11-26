--TEST--
Test expm1() - Error conditions
--INI--
precision=14
--SKIPIF--
<?php
	function_exists('expm1') or die('skip expm1() is not supported in this build.');
?>
--FILE--
<?php
/* Prototype  : float expm1  ( float $arg  )
 * Description: Returns exp(number) - 1, computed in a way that is accurate even 
 *              when the value of number is close to zero.
 * Source code: ext/standard/math.c
 */

echo "*** Testing expm1() : error conditions ***\n";

echo "\n-- Testing expm1() function with less than expected no. of arguments --\n";
expm1();
echo "\n-- Testing expm1() function with more than expected no. of arguments --\n";
expm1(23,true);

?>
===Done===
--EXPECTF--
*** Testing expm1() : error conditions ***

-- Testing expm1() function with less than expected no. of arguments --

Warning: expm1() expects exactly 1 parameter, 0 given in %s on line %d

-- Testing expm1() function with more than expected no. of arguments --

Warning: expm1() expects exactly 1 parameter, 2 given in %s on line %d
===Done===

