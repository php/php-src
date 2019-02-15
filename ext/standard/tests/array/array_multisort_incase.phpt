--TEST--
Test array_multisort() function : case-insensitive
--FILE--
<?php
/* Prototype  : bool array_multisort(array ar1 [, SORT_ASC|SORT_DESC [, SORT_REGULAR|SORT_NUMERIC|SORT_STRING|SORT_NATURAL|SORT_FLAG_CASE]] [, array ar2 [, SORT_ASC|SORT_DESC [, SORT_REGULAR|SORT_NUMERIC|SORT_STRING|SORT_NATURAL|SORT_FLAG_CASE], ...])
 * Description: Sort multiple arrays at once similar to how ORDER BY clause works in SQL
 * Source code: ext/standard/array.c
 * Alias to functions:
 */

echo "*** Testing array_multisort() : case-insensitive\n";

$a = array(
	'Second',
	'First.1',
	'First.2',
	'First.3',
	'Twentieth',
	'Tenth',
	'Third',
);

$b = array(
	'2 a',
	'1 bb 1',
	'1 bB 2',
	'1 BB 3',
	'20 c',
	'10 d',
	'3 e',
);

array_multisort($b, SORT_STRING | SORT_FLAG_CASE, $a);

var_dump($a, $b);

?>
===DONE===
--EXPECT--
*** Testing array_multisort() : case-insensitive
array(7) {
  [0]=>
  string(7) "First.1"
  [1]=>
  string(7) "First.2"
  [2]=>
  string(7) "First.3"
  [3]=>
  string(5) "Tenth"
  [4]=>
  string(6) "Second"
  [5]=>
  string(9) "Twentieth"
  [6]=>
  string(5) "Third"
}
array(7) {
  [0]=>
  string(6) "1 bb 1"
  [1]=>
  string(6) "1 bB 2"
  [2]=>
  string(6) "1 BB 3"
  [3]=>
  string(4) "10 d"
  [4]=>
  string(3) "2 a"
  [5]=>
  string(4) "20 c"
  [6]=>
  string(3) "3 e"
}
===DONE===
