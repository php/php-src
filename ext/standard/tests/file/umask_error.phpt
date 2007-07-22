--TEST--
Test umask() function: error conditions
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. only for Linux');
}
?>
--FILE--
<?php
/* Prototype: int umask ( [int $mask] );
   Description: Changes the current umask
*/

echo "*** Testing umask() : error conditions ***\n";

var_dump( umask(0000, true) );  // args > expected

echo "Done\n";
?>
--EXPECTF--
*** Testing umask() : error conditions ***

Warning: Wrong parameter count for umask() in %s on line %d
NULL
Done
