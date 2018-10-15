--TEST--
Test array_merge() function : usage variations - position of internal array pointer
--FILE--
<?php
/* Prototype  : array array_merge(array $arr1, array $arr2 [, array $...])
 * Description: Merges elements from passed arrays into one array
 * Source code: ext/standard/array.c
 */

/*
 * Check the position of the internal array pointer after calling array_merge().
 * This test is also passing more than two arguments to array_merge().
 */

echo "*** Testing array_merge() : usage variations ***\n";

$arr1 = array ('zero', 'one', 'two');
$arr2 = array ('zero', 'un', 'deux');
$arr3 = array ('null', 'eins', 'zwei');

echo "\n-- Call array_merge() --\n";
var_dump($result = array_merge($arr1, $arr2, $arr3));

echo "\n-- Position of Internal Pointer in Result: --\n";
echo key($result) . " => " . current($result) . "\n";

echo "\n-- Position of Internal Pointer in Original Array: --\n";
echo "\$arr1: ";
echo key($arr1) . " => " . current ($arr1) . "\n";
echo "\$arr2: ";
echo key($arr2) . " => " . current ($arr2) . "\n";
echo "\$arr3: ";
echo key($arr3) . " => " . current ($arr3) . "\n";

echo "Done";
?>
--EXPECTF--
*** Testing array_merge() : usage variations ***

-- Call array_merge() --
array(9) {
  [0]=>
  string(4) "zero"
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
  [3]=>
  string(4) "zero"
  [4]=>
  string(2) "un"
  [5]=>
  string(4) "deux"
  [6]=>
  string(4) "null"
  [7]=>
  string(4) "eins"
  [8]=>
  string(4) "zwei"
}

-- Position of Internal Pointer in Result: --
0 => zero

-- Position of Internal Pointer in Original Array: --
$arr1: 0 => zero
$arr2: 0 => zero
$arr3: 0 => null
Done
