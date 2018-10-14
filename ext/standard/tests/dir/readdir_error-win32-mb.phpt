--TEST--
Test readdir() function : error conditions - Incorrect number of args
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
  die("skip Valid only on Windows");
}
?>
--FILE--
<?php
/* Prototype  : string readdir([resource $dir_handle])
 * Description: Read directory entry from dir_handle
 * Source code: ext/standard/dir.c
 */

/*
 * Pass incorrect number of arguments to readdir() to test behaviour
 */

echo "*** Testing readdir() : error conditions ***\n";


//Test readdir with one more than the expected number of arguments
echo "\n-- Testing readdir() function with more than expected no. of arguments --\n";

$path = dirname(__FILE__) . "/私はガラスを食べられますreaddir_error";
mkdir($path);
$dir_handle = opendir($path);
$extra_arg = 10;

var_dump( readdir($dir_handle, $extra_arg) );

// close the handle so can remove dir in CLEAN section
closedir($dir_handle);
?>
===DONE===
--CLEAN--
<?php
$path = dirname(__FILE__) . "/私はガラスを食べられますreaddir_error";
rmdir($path);
?>
--EXPECTF--
*** Testing readdir() : error conditions ***

-- Testing readdir() function with more than expected no. of arguments --

Warning: readdir() expects at most 1 parameter, 2 given in %s on line %d
NULL
===DONE===
