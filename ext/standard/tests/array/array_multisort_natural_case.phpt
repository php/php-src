--TEST--
Test array_multisort() function : natural sorting case-sensitive
--FILE--
<?php
/* Prototype  : bool array_multisort(array ar1 [, SORT_ASC|SORT_DESC [, SORT_REGULAR|SORT_NUMERIC|SORT_STRING|SORT_NATURAL|SORT_FLAG_CASE]] [, array ar2 [, SORT_ASC|SORT_DESC [, SORT_REGULAR|SORT_NUMERIC|SORT_STRING|SORT_NATURAL|SORT_FLAG_CASE], ...])
 * Description: Sort multiple arrays at once similar to how ORDER BY clause works in SQL
 * Source code: ext/standard/array.c
 * Alias to functions:
 */

echo "*** Testing array_multisort() : natural sorting case-sensitive\n";

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

array_multisort($b, SORT_NATURAL, $a);

var_dump($a, $b);

?>
===DONE===
--EXPECTF--
*** Testing array_multisort() : natural sorting case-sensitive
array(7) {
  [0]=>
  string(7) "First.3"
  [1]=>
  string(7) "First.2"
  [2]=>
  string(7) "First.1"
  [3]=>
  string(6) "Second"
  [4]=>
  string(5) "Third"
  [5]=>
  string(5) "Tenth"
  [6]=>
  string(9) "Twentieth"
}
array(7) {
  [0]=>
  string(6) "1 BB 3"
  [1]=>
  string(6) "1 bB 2"
  [2]=>
  string(6) "1 bb 1"
  [3]=>
  string(3) "2 a"
  [4]=>
  string(3) "3 e"
  [5]=>
  string(4) "10 d"
  [6]=>
  string(4) "20 c"
}
===DONE===
