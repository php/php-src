--TEST--
Test array_multisort() function : natural sorting
--FILE--
<?php
echo "*** Testing array_multisort() : natural sorting\n";

$a = array(
    'Second',
    'First',
    'Twentieth',
    'Tenth',
    'Third',
);

$b = array(
    '2 a',
    '1 b',
    '20 c',
    '10 d',
    '3 e',
);

array_multisort($b, SORT_NATURAL, $a);

var_dump($a, $b);

?>
--EXPECT--
*** Testing array_multisort() : natural sorting
array(5) {
  [0]=>
  string(5) "First"
  [1]=>
  string(6) "Second"
  [2]=>
  string(5) "Third"
  [3]=>
  string(5) "Tenth"
  [4]=>
  string(9) "Twentieth"
}
array(5) {
  [0]=>
  string(3) "1 b"
  [1]=>
  string(3) "2 a"
  [2]=>
  string(3) "3 e"
  [3]=>
  string(4) "10 d"
  [4]=>
  string(4) "20 c"
}
