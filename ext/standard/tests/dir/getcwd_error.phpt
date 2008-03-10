--TEST--
Test getcwd() function : error conditions - Incorrect number of arguments
--FILE--
<?php
/* Prototype  : mixed getcwd(void)
 * Description: Gets the current directory 
 * Source code: ext/standard/dir.c
 */

/*
 * Pass incorrect number of arguments to getcwd() to test behaviour
 */

echo "*** Testing getcwd() : error conditions ***\n";

// One argument
echo "\n-- Testing getcwd() function with one argument --\n";
$extra_arg = 10;
var_dump( getcwd($extra_arg) );
?>
===DONE===
--EXPECTF--
*** Testing getcwd() : error conditions ***

-- Testing getcwd() function with one argument --

Warning: Wrong parameter count for getcwd() in %s on line %d
NULL
===DONE===
