--TEST--
Test lstat() and stat() functions: error conditions
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. lstat() not available on Windows');
}
?>
--FILE--
<?php
/* Prototype: array lstat ( string $filename );
   Description: Gives information about a file or symbolic link

   Prototype: array stat ( string $filename );
   Description: Gives information about a file
*/

echo "*** Testing lstat() for error conditions ***\n";
$file_path = dirname(__FILE__); 
var_dump( lstat() );  // args < expected
var_dump( lstat(__FILE__, 2) );  // args > expected
var_dump( lstat("$file_path/temp.tmp") ); // non existing file
var_dump( lstat(22) ); // scalar argument
$arr = array(__FILE__);
var_dump( lstat($arr) ); // array argument

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
*** Testing lstat() for error conditions ***

Warning: Wrong parameter count for lstat() in %s on line %d
NULL

Warning: Wrong parameter count for lstat() in %s on line %d
NULL

Warning: lstat(): Lstat failed for %s in %s on line %d
bool(false)

Warning: lstat(): Lstat failed for 22 in %s on line %d
bool(false)

Notice: Array to string conversion in %s on line %d

Warning: lstat(): Lstat failed for Array in %s on line %d
bool(false)

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
