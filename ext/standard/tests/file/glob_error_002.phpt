--TEST--
Test glob() function: long pattern.
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip Not valid for Windows");
?>
--FILE--
<?php
/* Prototype: array glob ( string $pattern [, int $flags] );
   Description: Find pathnames matching a pattern
*/

echo "*** Testing glob() : long pattern. ***\n";

var_dump(glob(str_repeat('x', 3000)));

echo "Done";
?>
--EXPECTF--
*** Testing glob() : long pattern. ***
array(0) {
}
Done

