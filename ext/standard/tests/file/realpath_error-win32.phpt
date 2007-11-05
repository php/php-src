--TEST--
Test realpath() function: error conditions
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip only on Windows');
}
?>
--FILE--
<?php
/* Prototype: string realpath ( string $path );
   Description: Returns canonicalized absolute pathname
*/

echo "*** Testing realpath() for error conditions ***\n";
var_dump( realpath() );  // args < expected
var_dump( realpath(1, 2) );  // args > expected

echo "Done\n";
?>
--EXPECTF--
*** Testing realpath() for error conditions ***

Warning: realpath() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: realpath() expects exactly 1 parameter, 2 given in %s on line %d
NULL
Done
