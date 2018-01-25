--TEST--
Test opendir() function : error conditions - Incorrect number of args
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
  die("skip Valid only on Windows");
}
?>
--FILE--
<?php
/* Prototype  : mixed opendir(string $path[, resource $context])
 * Description: Open a directory and return a dir_handle
 * Source code: ext/standard/dir.c
 */

/*
 * Pass incorrect number of arguments to opendir() to test behaviour
 */

echo "*** Testing opendir() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing opendir() function with Zero arguments --\n";
var_dump( opendir() );

//Test opendir with one more than the expected number of arguments
echo "\n-- Testing opendir() function with more than expected no. of arguments --\n";
$path = dirname(__FILE__) . "/私はガラスを食べられますopendir_error";
mkdir($path);
$context = stream_context_create();

$extra_arg = 10;
var_dump( opendir($path, $context, $extra_arg) );
?>
===DONE===
--CLEAN--
<?php
$path = dirname(__FILE__) . "/私はガラスを食べられますopendir_error";
rmdir($path);
?>
--EXPECTF--
*** Testing opendir() : error conditions ***

-- Testing opendir() function with Zero arguments --

Warning: opendir() expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing opendir() function with more than expected no. of arguments --

Warning: opendir() expects at most 2 parameters, 3 given in %s on line %d
NULL
===DONE===
