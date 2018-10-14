--TEST--
Test imagecolorstotal() function : error conditions  - Pass incorrect number of arguments
--SKIPIF--
<?php
if(!extension_loaded('gd')) {
    die('skip gd extension is not loaded');
}
if(!function_exists('imagecolorstotal')) {
    die('skip imagecolorstotal function is not available');
}
?>
--FILE--
<?php
/* Prototype  : int imagecolorstotal  ( resource $image  )
 * Description: Find out the number of colors in an image's palette
 * Source code: ext/gd/gd.c
 */

echo "*** Testing imagecolorstotal() : error conditions ***\n";

// Get a resource
$im = fopen(__FILE__, 'r');

echo "\n-- Testing imagecolorstotal() function with Zero arguments --\n";
var_dump( imagecolorstotal() );

echo "\n-- Testing imagecolorstotal() function with more than expected no. of arguments --\n";
$extra_arg = false;
var_dump( imagecolorstotal($im, $extra_arg) );

echo "\n-- Testing imagecolorstotal() function with a invalid resource\n";
var_dump( imagecolorstotal($im) );

fclose($im);
?>
===DONE===
--EXPECTF--
*** Testing imagecolorstotal() : error conditions ***

-- Testing imagecolorstotal() function with Zero arguments --

Warning: imagecolorstotal() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing imagecolorstotal() function with more than expected no. of arguments --

Warning: imagecolorstotal() expects exactly 1 parameter, 2 given in %s on line %d
NULL

-- Testing imagecolorstotal() function with a invalid resource

Warning: imagecolorstotal(): supplied resource is not a valid Image resource in %s on line %d
bool(false)
===DONE===
