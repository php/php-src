--TEST--
Test readlink() and realpath() functions: error conditions
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip only for Windows');
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
var_dump( readlink(__DIR__."/readlink_error.tmp") );

echo "\n*** Testing readlink() on existing file ***\n";
var_dump( readlink(__FILE__) );

echo "\n*** Testing readlink() on existing directory ***\n";
var_dump( readlink(__DIR__) );

echo "*** Testing realpath(): error conditions ***\n";
var_dump( realpath() );  // args < expected
var_dump( realpath(1, 2) );  // args > expected

echo "\n*** Testing realpath() on a non-existent file ***\n";
var_dump( realpath(__DIR__."/realpath_error.tmp") );

echo "Done\n";
?>
--EXPECTF--
*** Testing readlink(): error conditions ***

Warning: readlink() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: readlink() expects exactly 1 parameter, 2 given in %s on line %d
NULL

*** Testing readlink() on a non-existent link ***

Warning: readlink(): readlink failed to read the symbolic link (%s, error %d) in %s on line %d
bool(false)

*** Testing readlink() on existing file ***
string(%d) "%s%eext%estandard%etests%efile%ereadlink_realpath_error-win32.php"

*** Testing readlink() on existing directory ***
string(%d) "%s%eext%estandard%etests%efile"
*** Testing realpath(): error conditions ***

Warning: realpath() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: realpath() expects exactly 1 parameter, 2 given in %s on line %d
NULL

*** Testing realpath() on a non-existent file ***
%s
Done
