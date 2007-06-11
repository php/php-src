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

Warning: Wrong parameter count for realpath() in %s on line %d
NULL

Warning: Wrong parameter count for realpath() in %s on line %d
NULL
Done
