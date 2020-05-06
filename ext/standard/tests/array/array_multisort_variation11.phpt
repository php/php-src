--TEST--
Test array_multisort() function : usage variation - testing with empty array
--FILE--
<?php
/* Description: Sort multiple arrays at once similar to how ORDER BY clause works in SQL
 * Source code: ext/standard/array.c
 * Alias to functions:
 */

echo "*** Testing array_multisort() : Testing with empty array ***\n";

var_dump(array_multisort(array()));

?>
--EXPECT--
*** Testing array_multisort() : Testing with empty array ***
bool(true)
