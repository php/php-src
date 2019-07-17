--TEST--
Test mkdir() and rmdir() functions : error conditions
--FILE--
<?php
/*  Prototype: bool mkdir ( string $pathname [, int $mode [, bool $recursive [, resource $context]]] );
    Description: Makes directory

    Prototype: bool rmdir ( string $dirname [, resource $context] );
    Description: Removes directory
*/

echo "\n*** Testing rmdir() on non-existent directory ***\n";
var_dump( rmdir("temp") );

echo "Done\n";
?>
--EXPECTF--
*** Testing rmdir() on non-existent directory ***

Warning: rmdir(temp): No such file or directory in %s on line %d
bool(false)
Done
