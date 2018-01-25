--TEST--
Test array_multisort() function : usage variation - testing with anonymous arrary arguments
--FILE--
<?php
/* Prototype  : bool array_multisort(array ar1 [, SORT_ASC|SORT_DESC [, SORT_REGULAR|SORT_NUMERIC|SORT_STRING|SORT_NATURAL|SORT_FLAG_CASE]] [, array ar2 [, SORT_ASC|SORT_DESC [, SORT_REGULAR|SORT_NUMERIC|SORT_STRING|SORT_NATURAL|SORT_FLAG_CASE]], ...])
 * Description: Sort multiple arrays at once similar to how ORDER BY clause works in SQL
 * Source code: ext/standard/array.c
 * Alias to functions:
 */

echo "*** Testing array_multisort() : Testing with anonymous arguments ***\n";

var_dump(array_multisort(array(1,3,2,4)));

?>
===DONE===
--EXPECTF--
*** Testing array_multisort() : Testing with anonymous arguments ***
bool(true)
===DONE===
