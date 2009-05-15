--TEST--
Test array_multisort() function : usage variation - testing with empty array
--FILE--
<?php
/* Prototype  : bool array_multisort(array ar1 [, SORT_ASC|SORT_DESC [, SORT_REGULAR|SORT_NUMERIC|SORT_STRING]] [, array ar2 [, SORT_ASC|SORT_DESC [, SORT_REGULAR|SORT_NUMERIC|SORT_STRING]], ...])
 * Description: Sort multiple arrays at once similar to how ORDER BY clause works in SQL
 * Source code: ext/standard/array.c
 * Alias to functions:
 */

echo "*** Testing array_multisort() : Testing with empty array ***\n";

var_dump(array_multisort(array()));

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
*** Testing array_multisort() : Testing with empty array ***
bool(true)
===DONE===
