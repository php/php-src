--TEST--
Test gd_info() function : error conditions - with more than expected number of arguments
--SKIPIF--
<?php
if(!extension_loaded('gd')) {
    die('skip gd extension is not loaded');
}
if(!function_exists('gd_info')) {
    die('skip gd_info function is not available');
}
?>
--FILE--
<?php
/* Prototype  : array gd_info()
 * Description: Retrieve information about the currently installed GD library
 * Source code: ext/gd/gd.c
 */
$extra_arg_number = 10;
$extra_arg_string = "Hello";

echo "*** Testing gd_info() : error conditions ***\n";

echo "\n-- Testing gd_info() function with more than expected number of arguments --\n";
var_dump(gd_info($extra_arg_number));
var_dump(gd_info($extra_arg_string, $extra_arg_number));
?>
===DONE===
--EXPECTF--
*** Testing gd_info() : error conditions ***

-- Testing gd_info() function with more than expected number of arguments --

Warning: gd_info() expects exactly 0 parameters, 1 given in %s on line %d
bool(false)

Warning: gd_info() expects exactly 0 parameters, 2 given in %s on line %d
bool(false)
===DONE===