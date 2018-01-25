--TEST--
Test scandir() function : error conditions - Incorrect number of args
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
  die("skip Valid only on Windows");
}
?>
--FILE--
<?php
/* Prototype  : array scandir(string $dir [, int $sorting_order [, resource $context]])
 * Description: List files & directories inside the specified path
 * Source code: ext/standard/dir.c
 */

/*
 * Pass incorrect number of arguments to scandir() to test behaviour
 */

echo "*** Testing scandir() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing scandir() function with Zero arguments --\n";
var_dump( scandir() );

//Test scandir with one more than the expected number of arguments
echo "\n-- Testing scandir() function with more than expected no. of arguments --\n";
$dir = dirname(__FILE__) . '/私はガラスを食べられますscandir_error';
mkdir($dir);
$sorting_order = 10;
$context = stream_context_create();
$extra_arg = 10;
var_dump( scandir($dir, $sorting_order, $context, $extra_arg) );
?>
===DONE===
--CLEAN--
<?php
$directory = dirname(__FILE__) . '/私はガラスを食べられますscandir_error';
rmdir($directory);
?>
--EXPECTF--
*** Testing scandir() : error conditions ***

-- Testing scandir() function with Zero arguments --

Warning: scandir() expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing scandir() function with more than expected no. of arguments --

Warning: scandir() expects at most 3 parameters, 4 given in %s on line %d
NULL
===DONE===
