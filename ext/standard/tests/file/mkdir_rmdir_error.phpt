--TEST--
Test mkdir() and rmdir() functions : error conditions
--FILE--
<?php
/*  Prototype: bool mkdir ( string $pathname [, int $mode [, bool $recursive [, resource $context]]] );
    Description: Makes directory

    Prototype: bool rmdir ( string $dirname [, resource $context] );
    Description: Removes directory
*/

echo "*** Testing mkdir(): error conditions ***\n";
var_dump( mkdir() );  // args < expected
var_dump( mkdir(1, 2, 3, 4, 5) );  // args > expected
var_dump( mkdir("testdir", 0777, false, $context, "test") );  // args > expected

echo "\n*** Testing rmdir(): error conditions ***\n";
var_dump( rmdir() );  // args < expected
var_dump( rmdir(1, 2, 3) );  // args > expected
var_dump( rmdir("testdir", $context, "test") );  // args > expected

echo "\n*** Testing rmdir() on non-existent directory ***\n";
var_dump( rmdir("temp") );

echo "Done\n";
?>
--EXPECTF--
*** Testing mkdir(): error conditions ***

Warning: mkdir() expects at least 1 parameter, 0 given in %s on line %d
bool(false)

Warning: mkdir() expects at most 4 parameters, 5 given in %s on line %d
bool(false)

Notice: Undefined variable: context in %s on line %d

Warning: mkdir() expects at most 4 parameters, 5 given in %s on line %d
bool(false)

*** Testing rmdir(): error conditions ***

Warning: rmdir() expects at least 1 parameter, 0 given in %s on line %d
bool(false)

Warning: rmdir() expects at most 2 parameters, 3 given in %s on line %d
bool(false)

Notice: Undefined variable: context in %s on line %d

Warning: rmdir() expects at most 2 parameters, 3 given in %s on line %d
bool(false)

*** Testing rmdir() on non-existent directory ***

Warning: rmdir(temp): No such file or directory in %s on line %d
bool(false)
Done
