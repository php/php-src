--TEST--
Test rewinddir() function : error conditions - incorrect number of args
--FILE--
<?php
/* Prototype  : void rewinddir([resource $dir_handle])
 * Description: Rewind dir_handle back to the start
 * Source code: ext/standard/dir.c
 * Alias to functions: rewind
 */

/*
 * Pass incorrect number of arguments to rewinddir() to test behaviour
 */

echo "*** Testing rewinddir() : error conditions ***\n";


//Test rewinddir with one more than the expected number of arguments
echo "\n-- Testing rewinddir() function with more than expected no. of arguments --\n";

$dir_path = dirname(__FILE__) . "/rewinddir_error";
mkdir($dir_path);
$dir_handle = opendir($dir_path);
$extra_arg = 10;

var_dump( rewinddir($dir_handle, $extra_arg) );
closedir($dir_handle);
?>
===DONE===
--CLEAN--
<?php
$dir_path = dirname(__FILE__) . "/rewinddir_error";
rmdir($dir_path);
?>
--EXPECTF--
*** Testing rewinddir() : error conditions ***

-- Testing rewinddir() function with more than expected no. of arguments --

Warning: rewinddir() expects at most 1 parameter, 2 given in %s on line %d
NULL
===DONE===
