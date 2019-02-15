--TEST--
Test closedir() function : error conditions - Pass incorrect number of arguments
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
  die("skip Valid only on Windows");
}
?>
--FILE--
<?php
/* Prototype  : void closedir([resource $dir_handle])
 * Description: Close directory connection identified by the dir_handle
 * Source code: ext/standard/dir.c
 * Alias to functions: close
 */

/*
 * Pass incorrect number of arguments to closedir() to test behaviour
 */

echo "*** Testing closedir() : error conditions ***\n";


//Test closedir with one more than the expected number of arguments
echo "\n-- Testing closedir() function with more than expected no. of arguments --\n";

$dir_path = dirname(__FILE__) . '\私はガラスを食べられますclosedir_error';
mkdir($dir_path);
$dir_handle = opendir($dir_path);

$extra_arg = 10;
var_dump( closedir($dir_handle, $extra_arg) );

//successfully close the directory handle so can delete in CLEAN section
closedir($dir_handle);
?>
===DONE===
--CLEAN--
<?php
$base_dir = dirname(__FILE__);
$dir_path = $base_dir . '\私はガラスを食べられますclosedir_error';
rmdir($dir_path);
?>
--EXPECTF--
*** Testing closedir() : error conditions ***

-- Testing closedir() function with more than expected no. of arguments --

Warning: closedir() expects at most 1 parameter, 2 given in %s on line %d
NULL
===DONE===
