--TEST--
Test readlink() and realpath() functions: error conditions
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip no symlinks on Windows');
}
?>
--FILE--
<?php
/* Prototype: string readlink ( string $path );
   Description: Returns the target of a symbolic link

   Prototype: string realpath ( string $path );
   Description: Returns canonicalized absolute pathname
*/

echo "*** Testing readlink(): error conditions ***\n";
var_dump( readlink() );  // args < expected
var_dump( readlink(__FILE__, 2) );  // args > expected

echo "\n*** Testing readlink() on a non-existent link ***\n";
var_dump( readlink(dirname(__FILE__)."/readlink_error.tmp") );

echo "\n*** Testing readlink() on existing file ***\n";
var_dump( readlink(__FILE__) );

echo "\n*** Testing readlink() on existing directory ***\n";
var_dump( readlink(dirname(__FILE__)) );

echo "*** Testing realpath(): error conditions ***\n";
var_dump( realpath() );  // args < expected
var_dump( realpath(1, 2) );  // args > expected

echo "\n*** Testing realpath() on a non-existent file ***\n";
var_dump( realpath(dirname(__FILE__)."/realpath_error.tmp") );

echo "Done\n";
?>
--EXPECTF--
*** Testing readlink(): error conditions ***

Warning: Wrong parameter count for readlink() in %s on line %d
NULL

Warning: Wrong parameter count for readlink() in %s on line %d
NULL

*** Testing readlink() on a non-existent link ***

Warning: readlink(): No such file or directory in %s on line %d
bool(false)

*** Testing readlink() on existing file ***

Warning: readlink(): Invalid argument in %s on line %d
bool(false)

*** Testing readlink() on existing directory ***

Warning: readlink(): Invalid argument in %s on line %d
bool(false)
*** Testing realpath(): error conditions ***

Warning: Wrong parameter count for realpath() in %s on line %d
NULL

Warning: Wrong parameter count for realpath() in %s on line %d
NULL

*** Testing realpath() on a non-existent file ***
%s
Done
