--TEST--
Test stat() function: error conditions
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip.. only for Windows');
}
?>
--FILE--
<?php
/*
   Prototype: array stat ( string $filename );
   Description: Gives information about a file
*/

$file_path = dirname(__FILE__); 
$arr = array(__FILE__);

echo "\n*** Testing stat() for error conditions ***\n";
var_dump( stat() );  // args < expected
var_dump( stat(__FILE__, 2) );  // file, args > expected
var_dump( stat(dirname(__FILE__), 2) );  //dir, args > expected

var_dump( stat("$file_path/temp.tmp") ); // non existing file
var_dump( stat("$file_path/temp/") ); // non existing dir
var_dump( stat(22) ); // scalar argument
var_dump( stat($arr) ); // array argument

echo "Done\n";
?>
--EXPECTF--
*** Testing stat() for error conditions ***

Warning: Wrong parameter count for stat() in %s on line %d
NULL

Warning: Wrong parameter count for stat() in %s on line %d
NULL

Warning: Wrong parameter count for stat() in %s on line %d
NULL

Warning: stat(): stat failed for %s in %s on line %d
bool(false)

Warning: stat(): stat failed for %s in %s on line %d
bool(false)

Warning: stat(): stat failed for 22 in %s on line %d
bool(false)

Notice: Array to string conversion in %s on line %d

Warning: stat(): stat failed for Array in %s on line %d
bool(false)
Done
